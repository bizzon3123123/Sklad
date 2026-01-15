#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <sstream>

using namespace std;

// ==================== СИСТЕМА СОБЫТИЙ (SIGNALS) ====================
class SignalSystem {
private:
    static SignalSystem* instance;
    mutex signalMutex;
    map<string, vector<function<void(int, int, string)>>> signals;
    
    SignalSystem() {}
    
public:
    static SignalSystem& get() {
        if (!instance) {
            instance = new SignalSystem();
        }
        return *instance;
    }
    
    void connect(const string& signal, function<void(int, int, string)> slot) {
        lock_guard<mutex> lock(signalMutex);
        signals[signal].push_back(slot);
    }
    
    void emit(const string& signal, int productId, int quantity, string action) {
        lock_guard<mutex> lock(signalMutex);
        if (signals.find(signal) != signals.end()) {
            for (auto& slot : signals[signal]) {
                slot(productId, quantity, action);
            }
        }
    }
};

SignalSystem* SignalSystem::instance = nullptr;

// ==================== БАЗОВЫЕ КЛАССЫ ПО UML ====================

// 1. Класс Product (Товар)
class Product {
private:
    int id;
    string name;
    double price;
    int quantity;
    mutable mutex productMutex;
    
public:
    Product(int _id, string _name, double _price, int _quantity = 0)
        : id(_id), name(_name), price(_price), quantity(_quantity) {}
    
    // Геттеры с мьютексами
    int getId() const { lock_guard<mutex> lock(productMutex); return id; }
    string getName() const { lock_guard<mutex> lock(productMutex); return name; }
    double getPrice() const { lock_guard<mutex> lock(productMutex); return price; }
    int getQuantity() const { lock_guard<mutex> lock(productMutex); return quantity; }
    void setQuantity(int qty) { lock_guard<mutex> lock(productMutex); quantity = qty; }
    
    // ПЕРЕГРУЗКА ОПЕРАТОРОВ ++ и -- (из задания)
    Product& operator++() {
        lock_guard<mutex> lock(productMutex);
        quantity++;
        SignalSystem::get().emit("product_incremented", id, 1, "Увеличение на складе");
        return *this;
    }
    
    Product operator++(int) {
        lock_guard<mutex> lock(productMutex);
        Product temp = *this;
        quantity++;
        SignalSystem::get().emit("product_incremented", id, 1, "Увеличение на складе");
        return temp;
    }
    
    Product& operator--() {
        lock_guard<mutex> lock(productMutex);
        if (quantity > 0) {
            quantity--;
            SignalSystem::get().emit("product_decremented", id, 1, "Уменьшение на складе");
        }
        return *this;
    }
    
    Product operator--(int) {
        lock_guard<mutex> lock(productMutex);
        Product temp = *this;
        if (quantity > 0) {
            quantity--;
            SignalSystem::get().emit("product_decremented", id, 1, "Уменьшение на складе");
        }
        return temp;
    }
    
    bool changeQuantity(int delta) {
        lock_guard<mutex> lock(productMutex);
        if (quantity + delta >= 0) {
            quantity += delta;
            return true;
        }
        return false;
    }
};

// 2. Класс Warehouse (Склад)
class Warehouse {
private:
    int id;
    string name;
    vector<shared_ptr<Product>> products;
    mutable mutex warehouseMutex;
    
public:
    Warehouse(int _id, string _name) : id(_id), name(_name) {
        // Подписываемся на сигналы
        auto& signals = SignalSystem::get();
        signals.connect("order_item_added", 
            [this](int pid, int qty, string action) { onOrderItemAdded(pid, qty); });
        signals.connect("order_item_removed",
            [this](int pid, int qty, string action) { onOrderItemRemoved(pid, qty); });
    }
    
    void addProduct(shared_ptr<Product> product) {
        lock_guard<mutex> lock(warehouseMutex);
        products.push_back(product);
    }
shared_ptr<Product> findProduct(int productId) {
        lock_guard<mutex> lock(warehouseMutex);
        for (auto& product : products) {
            if (product->getId() == productId) {
                return product;
            }
        }
        return nullptr;
    }
    
    void onOrderItemAdded(int productId, int quantity) {
        auto product = findProduct(productId);
        if (product && product->changeQuantity(-quantity)) {
            cout << "[СКЛАД] Списано: " << product->getName() 
                 << " × " << quantity << endl;
        }
    }
    
    void onOrderItemRemoved(int productId, int quantity) {
        auto product = findProduct(productId);
        if (product) {
            product->changeQuantity(quantity);
            cout << "[СКЛАД] Возвращено: " << product->getName() 
                 << " × " << quantity << endl;
        }
    }
    
    void printInventory() const {
        lock_guard<mutex> lock(warehouseMutex);
        cout << "\n=== ИНВЕНТАРЬ СКЛАДА '" << name << "' ===" << endl;
        for (const auto& product : products) {
            cout << product->getName() << ": " << product->getQuantity() << " шт." << endl;
        }
    }
};

// 3. Класс ReceiptItem (Позиция в чеке) с перегрузкой операторов
class ReceiptItem {
private:
    shared_ptr<Product> product;
    int quantity;
    mutable mutex itemMutex;
    
public:
    ReceiptItem(shared_ptr<Product> _product, int _quantity = 1)
        : product(_product), quantity(_quantity) {}
    
    // ПЕРЕГРУЗКА ОПЕРАТОРОВ ++ и --
    ReceiptItem& operator++() {
        lock_guard<mutex> lock(itemMutex);
        quantity++;
        SignalSystem::get().emit("receipt_item_incremented", 
            product->getId(), 1, "Увеличение в чеке");
        return *this;
    }
    
    ReceiptItem operator++(int) {
        lock_guard<mutex> lock(itemMutex);
        ReceiptItem temp = *this;
        quantity++;
        SignalSystem::get().emit("receipt_item_incremented",
            product->getId(), 1, "Увеличение в чеке");
        return temp;
    }
    
    ReceiptItem& operator--() {
        lock_guard<mutex> lock(itemMutex);
        if (quantity > 0) {
            quantity--;
            SignalSystem::get().emit("receipt_item_decremented",
                product->getId(), 1, "Уменьшение в чеке");
        }
        return *this;
    }
    
    ReceiptItem operator--(int) {
        lock_guard<mutex> lock(itemMutex);
        ReceiptItem temp = *this;
        if (quantity > 0) {
            quantity--;
            SignalSystem::get().emit("receipt_item_decremented",
                product->getId(), 1, "Уменьшение в чеке");
        }
        return temp;
    }
    
    shared_ptr<Product> getProduct() const { 
        lock_guard<mutex> lock(itemMutex); 
        return product; 
    }
    
    int getQuantity() const { 
        lock_guard<mutex> lock(itemMutex); 
        return quantity; 
    }
    
    bool isEmpty() const { 
        lock_guard<mutex> lock(itemMutex); 
        return quantity == 0; 
    }
};

// 4. Класс Receipt (Чек)
class Receipt {
private:
    int id;
    vector<shared_ptr<ReceiptItem>> items;
    mutable mutex receiptMutex;
    
public:
    Receipt(int _id) : id(_id) {}
    
    // Использование перегруженных операторов
    void addProduct(shared_ptr<Product> product) {
        lock_guard<mutex> lock(receiptMutex);
        for (auto& item : items) {
            if (item->getProduct()->getId() == product->getId()) {
                ++(*item);  // Используем перегруженный ++
                return;
            }
        }
        items.push_back(make_shared<ReceiptItem>(product, 1));
        SignalSystem::get().emit("product_added_to_receipt", 
            product->getId(), 1, "Добавлен в чек");
    }
    
    void removeProduct(shared_ptr<Product> product) {
        lock_guard<mutex> lock(receiptMutex);
        for (auto it = items.begin(); it != items.end(); ++it) {
            if ((*it)->getProduct()->getId() == product->getId()) {
                --(**it);  // Используем перегруженный --
if ((*it)->isEmpty()) {
                    items.erase(it);
                }
                break;
            }
        }
    }
    
    void print() const {
        lock_guard<mutex> lock(receiptMutex);
        cout << "\n=== ЧЕК №" << id << " ===" << endl;
        for (const auto& item : items) {
            cout << item->getProduct()->getName() 
                 << " × " << item->getQuantity() << endl;
        }
    }
};

// ==================== ШАБЛОНЫ ДОКУМЕНТОВ ====================

enum class DocumentType {
    ORDER,
    INVOICE_IN,
    INVOICE_OUT
};

// ШАБЛОННЫЙ КЛАСС ДОКУМЕНТА (из задания)
template<DocumentType Type>
class DocumentTemplate {
protected:
    int id;
    string number;
    string createdBy;
    vector<pair<shared_ptr<Product>, int>> items;
    string status;
    mutable mutex documentMutex;
    
public:
    DocumentTemplate(int _id, string _num, string _creator)
        : id(_id), number(_num), createdBy(_creator), status("Черновик") {}
    
    virtual string getTypeName() const = 0;
    
    void addItem(shared_ptr<Product> product, int quantity = 1) {
        lock_guard<mutex> lock(documentMutex);
        
        // Проверяем, есть ли уже товар
        for (auto& item : items) {
            if (item.first->getId() == product->getId()) {
                item.second += quantity;
                
                // Отправляем сигнал для заказов
                if (Type == DocumentType::ORDER) {
                    SignalSystem::get().emit("order_item_added", 
                        product->getId(), quantity, "Увеличение в заказе");
                }
                return;
            }
        }
        
        // Добавляем новый товар
        items.push_back({product, quantity});
        
        // Отправляем сигнал для заказов
        if (Type == DocumentType::ORDER) {
            SignalSystem::get().emit("order_item_added", 
                product->getId(), quantity, "Добавлен в заказ");
        }
    }
    
    void removeItem(int productId, int quantity = 1) {
        lock_guard<mutex> lock(documentMutex);
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->first->getId() == productId) {
                int removeQty = min(quantity, it->second);
                
                if (Type == DocumentType::ORDER) {
                    SignalSystem::get().emit("order_item_removed",
                        productId, removeQty, "Удален из заказа");
                }
                
                if (it->second > removeQty) {
                    it->second -= removeQty;
                } else {
                    items.erase(it);
                }
                break;
            }
        }
    }
    
    virtual void process() {
        lock_guard<mutex> lock(documentMutex);
        status = "Обработан";
        cout << getTypeName() << " №" << number << " обработан" << endl;
    }
    
    void print() const {
        lock_guard<mutex> lock(documentMutex);
        cout << "\n=== " << getTypeName() << " №" << number << " ===" << endl;
        cout << "Статус: " << status << endl;
        cout << "Создал: " << createdBy << endl;
        cout << "Позиции:" << endl;
        for (const auto& item : items) {
            cout << "  " << item.first->getName() 
                 << " × " << item.second << endl;
        }
    }
};

// Специализации шаблона
class Order : public DocumentTemplate<DocumentType::ORDER> {
public:
    Order(int id, string num, string creator)
        : DocumentTemplate(id, num, creator) {}
    
    string getTypeName() const override { return "ЗАКАЗ"; }
};

class IncomeInvoice : public DocumentTemplate<DocumentType::INVOICE_IN> {
public:
    IncomeInvoice(int id, string num, string creator)
        : DocumentTemplate(id, num, creator) {}
    
    string getTypeName() const override { return "НАКЛАДНАЯ ПРИХОДА"; }
    
    void process() override {
        DocumentTemplate::process();
        // При поступлении увеличиваем количество на складе
        for (const auto& item : items) {
for (int i = 0; i < item.second; i++) {
                ++(*item.first);  // Используем перегруженный ++
            }
        }
    }
};

// ==================== МНОГОПОТОЧНАЯ ОБРАБОТКА ====================

class ThreadPool {
private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex queueMutex;
    condition_variable cv;
    atomic<bool> stopFlag;
    
public:
    ThreadPool(size_t numThreads) : stopFlag(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(queueMutex);
                        cv.wait(lock, [this]() { 
                            return stopFlag || !tasks.empty(); 
                        });
                        
                        if (stopFlag && tasks.empty()) return;
                        
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    
                    task();
                }
            });
        }
    }
    
    ~ThreadPool() {
        stopFlag = true;
        cv.notify_all();
        for (auto& worker : workers) {
            if (worker.joinable()) worker.join();
        }
    }
    
    template<typename F>
    void enqueue(F&& task) {
        {
            lock_guard<mutex> lock(queueMutex);
            tasks.emplace(forward<F>(task));
        }
        cv.notify_one();
    }
};

// ==================== МНОГОПОТОЧНЫЙ ЗАКАЗ ====================

class ConcurrentOrderProcessor {
private:
    shared_ptr<ThreadPool> threadPool;
    Warehouse& warehouse;
    
public:
    ConcurrentOrderProcessor(Warehouse& wh, size_t numThreads = 4)
        : warehouse(wh) {
        threadPool = make_shared<ThreadPool>(numThreads);
    }
    
    void processOrderAsync(shared_ptr<Order> order) {
        threadPool->enqueue([this, order]() {
            cout << "[ПОТОК] Начало обработки заказа №" << order->print() << endl;
            
            // Имитация обработки
            this_thread::sleep_for(chrono::milliseconds(100));
            
            order->process();
            
            cout << "[ПОТОК] Заказ №" << order->print() << " обработан" << endl;
        });
    }
};