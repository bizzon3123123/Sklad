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

using namespace std;

// ==================== МНОГОПОТОЧНЫЕ КОМПОНЕНТЫ ====================

// Мьютексы для потокобезопасности
mutex warehouseMutex;          // Для операций со складом
mutex orderMutex;              // Для операций с заказами
mutex receiptMutex;            // Для операций с чеками
mutex coutMutex;               // Для безопасного вывода в консоль

// Безопасный вывод в консоль
void safePrint(const string& message) {
    lock_guard<mutex> lock(coutMutex);
    cout << message << endl;
}

// ==================== СУЩЕСТВУЮЩИЕ КЛАССЫ С МЬЮТЕКСАМИ ====================

// Класс Товар с потокобезопасным доступом
class Product {
private:
    int id;
    string name;
    double price;
    int quantity;
    mutable mutex productMutex;  // Мьютекс для этого конкретного товара

public:
    Product(int _id, string _name, double _price, int _quantity = 0) 
        : id(_id), name(_name), price(_price), quantity(_quantity) {}

    int getId() const { 
        lock_guard<mutex> lock(productMutex);
        return id; 
    }
    
    string getName() const { 
        lock_guard<mutex> lock(productMutex);
        return name; 
    }
    
    double getPrice() const { 
        lock_guard<mutex> lock(productMutex);
        return price; 
    }
    
    int getQuantity() const { 
        lock_guard<mutex> lock(productMutex);
        return quantity; 
    }
    
    void setQuantity(int qty) { 
        lock_guard<mutex> lock(productMutex);
        quantity = qty; 
    }
    
    // Потокобезопасные операторы
    Product& operator++() {
        lock_guard<mutex> lock(productMutex);
        quantity++;
        return *this;
    }
    
    Product& operator--() {
        lock_guard<mutex> lock(productMutex);
        if (quantity > 0) quantity--;
        return *this;
    }
    
    // Метод для атомарного изменения количества
    bool changeQuantity(int delta) {
        lock_guard<mutex> lock(productMutex);
        if (quantity + delta >= 0) {
            quantity += delta;
            return true;
        }
        return false;
    }
};

// Класс Склад с многопоточным доступом
class Warehouse {
private:
    int id;
    string name;
    vector<shared_ptr<Product>> products;
    mutable mutex warehouseMutex;  // Свой мьютекс для склада

public:
    Warehouse(int _id, string _name) : id(_id), name(_name) {}

    // Потокобезопасное добавление товара
    void addProduct(shared_ptr<Product> product) {
        lock_guard<mutex> lock(warehouseMutex);
        products.push_back(product);
    }

    // Потокобезопасный поиск товара
    shared_ptr<Product> findProduct(int productId) {
        lock_guard<mutex> lock(warehouseMutex);
        for (auto& product : products) {
            if (product->getId() == productId) {
                return product;
            }
        }
        return nullptr;
    }

    // Потокобезопасное списание товара
    bool reserveProduct(int productId, int quantity) {
        lock_guard<mutex> lock(warehouseMutex);
        for (auto& product : products) {
            if (product->getId() == productId) {
                return product->changeQuantity(-quantity);
            }
        }
        return false;
    }

    // Потокобезопасное возвращение товара
    bool returnProduct(int productId, int quantity) {
        lock_guard<mutex> lock(warehouseMutex);
        for (auto& product : products) {
            if (product->getId() == productId) {
                return product->changeQuantity(quantity);
            }
        }
        return false;
    }

    // Потокобезопасный вывод инвентаря
    void printInventory() const {
        lock_guard<mutex> lock(warehouseMutex);
        safePrint("\n=== ИНВЕНТАРЬ СКЛАДА '" + name + "' ===");
        for (const auto& product : products) {
            safePrint(product->getName() + ": " + to_string(product->getQuantity()) + " шт.");
        }
    }
string getName() const {
        lock_guard<mutex> lock(warehouseMutex);
        return name;
    }
};

// ==================== СИСТЕМА ОЧЕРЕДИ ЗАДАЧ ====================

// Задача для обработки в отдельном потоке
struct WarehouseTask {
    enum Type { RESERVE, RETURN, CHECK };
    Type type;
    int productId;
    int quantity;
    function<void(bool)> callback;
    
    WarehouseTask(Type t, int pid, int qty, function<void(bool)> cb)
        : type(t), productId(pid), quantity(qty), callback(cb) {}
};

// Очередь задач для склада с многопоточным доступом
class WarehouseTaskQueue {
private:
    queue<WarehouseTask> tasks;
    mutex queueMutex;
    condition_variable cv;
    bool stopFlag = false;
    thread workerThread;
    
    void worker(Warehouse& warehouse) {
        while (true) {
            WarehouseTask task;
            
            {
                unique_lock<mutex> lock(queueMutex);
                cv.wait(lock, [this]() { return !tasks.empty() || stopFlag; });
                
                if (stopFlag && tasks.empty()) break;
                
                task = tasks.front();
                tasks.pop();
            }
            
            // Выполняем задачу
            bool success = false;
            switch(task.type) {
                case WarehouseTask::RESERVE:
                    success = warehouse.reserveProduct(task.productId, task.quantity);
                    safePrint("[ПОТОК] Выполнено списание товара ID " + to_string(task.productId) + 
                             " × " + to_string(task.quantity) + " (" + 
                             (success ? "успех" : "ошибка") + ")");
                    break;
                case WarehouseTask::RETURN:
                    success = warehouse.returnProduct(task.productId, task.quantity);
                    safePrint("[ПОТОК] Выполнен возврат товара ID " + to_string(task.productId) + 
                             " × " + to_string(task.quantity) + " (" + 
                             (success ? "успех" : "ошибка") + ")");
                    break;
                case WarehouseTask::CHECK:
                    auto product = warehouse.findProduct(task.productId);
                    success = (product != nullptr && product->getQuantity() >= task.quantity);
                    safePrint("[ПОТОК] Проверка товара ID " + to_string(task.productId) + 
                             " на наличие " + to_string(task.quantity) + " шт. (" + 
                             (success ? "доступно" : "недоступно") + ")");
                    break;
            }
            
            // Вызываем колбэк
            if (task.callback) {
                task.callback(success);
            }
        }
    }
    
public:
    WarehouseTaskQueue(Warehouse& warehouse) {
        workerThread = thread(&WarehouseTaskQueue::worker, this, ref(warehouse));
    }
    
    ~WarehouseTaskQueue() {
        {
            lock_guard<mutex> lock(queueMutex);
            stopFlag = true;
        }
        cv.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
    
    void addTask(WarehouseTask::Type type, int productId, int quantity, function<void(bool)> callback) {
        {
            lock_guard<mutex> lock(queueMutex);
            tasks.emplace(type, productId, quantity, callback);
        }
        cv.notify_one();
    }
};

// ==================== МНОГОПОТОЧНЫЙ ЗАКАЗ ====================

class ConcurrentOrder {
private:
    int id;
    vector<pair<shared_ptr<Product>, int>> items;
    mutex orderMutex;
    shared_ptr<WarehouseTaskQueue> taskQueue;
    
public:
    ConcurrentOrder(int _id, shared_ptr<WarehouseTaskQueue> queue) 
        : id(_id), taskQueue(queue) {}
    
    // Асинхронное добавление товара
    void addProductAsync(shared_ptr<Product> product, int quantity) {
        safePrint("[ЗАКАЗ " + to_string(id) + "] Запрос на добавление: " + 
                 product->getName() + " × " + to_string(quantity));
        
        // Добавляем задачу в очередь
taskQueue->addTask(WarehouseTask::RESERVE, product->getId(), quantity,
            [this, product, quantity](bool success) {
                if (success) {
                    lock_guard<mutex> lock(orderMutex);
                    items.push_back({product, quantity});
                    safePrint("[ЗАКАЗ " + to_string(id) + "] Товар добавлен: " + 
                             product->getName() + " × " + to_string(quantity));
                } else {
                    safePrint("[ЗАКАЗ " + to_string(id) + "] Ошибка: недостаточно товара " + 
                             product->getName() + " на складе");
                }
            });
    }
    
    // Асинхронное удаление товара
    void removeProductAsync(int productId, int quantity) {
        safePrint("[ЗАКАЗ " + to_string(id) + "] Запрос на удаление товара ID " + 
                 to_string(productId) + " × " + to_string(quantity));
        
        taskQueue->addTask(WarehouseTask::RETURN, productId, quantity,
            [this, productId, quantity](bool success) {
                if (success) {
                    lock_guard<mutex> lock(orderMutex);
                    // Ищем и удаляем товар из заказа
                    for (auto it = items.begin(); it != items.end(); ++it) {
                        if (it->first->getId() == productId) {
                            if (it->second > quantity) {
                                it->second -= quantity;
                            } else {
                                items.erase(it);
                            }
                            break;
                        }
                    }
                    safePrint("[ЗАКАЗ " + to_string(id) + "] Товар удален из заказа");
                }
            });
    }
    
    // Проверка наличия товара
    void checkProductAsync(int productId, int quantity) {
        taskQueue->addTask(WarehouseTask::CHECK, productId, quantity,
            [this, productId, quantity](bool available) {
                safePrint("[ЗАКАЗ " + to_string(id) + "] Товар ID " + to_string(productId) + 
                         " доступен: " + to_string(quantity) + " шт. - " + 
                         (available ? "ДА" : "НЕТ"));
            });
    }
    
    void print() {
        lock_guard<mutex> lock(orderMutex);
        safePrint("\n=== ЗАКАЗ №" + to_string(id) + " ===");
        safePrint("Позиции:");
        for (const auto& item : items) {
            safePrint("  " + item.first->getName() + " × " + to_string(item.second));
        }
    }
};

// ==================== МНОГОПОТОЧНАЯ ОБРАБОТКА ЧЕКОВ ====================

class ConcurrentReceiptProcessor {
private:
    queue<pair<int, vector<pair<int, int>>>> receiptsQueue;  // очередь чеков
    mutex receiptQueueMutex;
    condition_variable receiptCV;
    vector<thread> workerThreads;
    bool stopFlag = false;
    Warehouse& warehouse;
    
    void receiptWorker(int workerId) {
        while (true) {
            pair<int, vector<pair<int, int>>> receipt;
            
            {
                unique_lock<mutex> lock(receiptQueueMutex);
                receiptCV.wait(lock, [this]() { return !receiptsQueue.empty() || stopFlag; });
                
                if (stopFlag && receiptsQueue.empty()) {
                    safePrint("[КАССИР " + to_string(workerId) + "] Завершение работы");
                    break;
                }
                
                receipt = receiptsQueue.front();
                receiptsQueue.pop();
            }
            
            // Обрабатываем чек
            safePrint("[КАССИР " + to_string(workerId) + "] Обработка чека №" + 
                     to_string(receipt.first));
            
            for (const auto& item : receipt.second) {
                this_thread::sleep_for(chrono::milliseconds(100)); // Имитация работы
                warehouse.reserveProduct(item.first, item.second);
                safePrint("[КАССИР " + to_string(workerId) + "] Списано: товар ID " + 
                         to_string(item.first) + " × " + to_string(item.second));
}
            
            safePrint("[КАССИР " + to_string(workerId) + "] Чек №" + 
                     to_string(receipt.first) + " обработан");
        }
    }
    
public:
    ConcurrentReceiptProcessor(Warehouse& wh, int numWorkers = 3) : warehouse(wh) {
        for (int i = 0; i < numWorkers; i++) {
            workerThreads.emplace_back(&ConcurrentReceiptProcessor::receiptWorker, this, i + 1);
        }
    }
    
    ~ConcurrentReceiptProcessor() {
        {
            lock_guard<mutex> lock(receiptQueueMutex);
            stopFlag = true;
        }
        receiptCV.notify_all();
        for (auto& thread : workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    
    void addReceipt(int receiptId, const vector<pair<int, int>>& items) {
        {
            lock_guard<mutex> lock(receiptQueueMutex);
            receiptsQueue.push({receiptId, items});
        }
        receiptCV.notify_one();
    }
};

// ==================== ДЕМОНСТРАЦИЯ МНОГОПОТОЧНОЙ РАБОТЫ ====================

int main() {
    safePrint("=== МНОГОПОТОЧНАЯ СИСТЕМА УПРАВЛЕНИЯ СКЛАДОМ ===\n");
    
    // 1. Создаем склад
    Warehouse warehouse(1, "Основной склад");
    
    // 2. Добавляем товары
    auto laptop = make_shared<Product>(1, "Ноутбук", 50000, 100);
    auto mouse = make_shared<Product>(2, "Мышь", 1500, 200);
    auto keyboard = make_shared<Product>(3, "Клавиатура", 3000, 150);
    
    warehouse.addProduct(laptop);
    warehouse.addProduct(mouse);
    warehouse.addProduct(keyboard);
    
    warehouse.printInventory();
    
    // 3. Создаем очередь задач для склада
    auto taskQueue = make_shared<WarehouseTaskQueue>(warehouse);
    
    // 4. Создаем несколько заказов, которые работают параллельно
    vector<unique_ptr<ConcurrentOrder>> orders;
    vector<thread> orderThreads;
    
    // Заказ 1 - работает в отдельном потоке
    orderThreads.emplace_back([&]() {
        auto order1 = make_unique<ConcurrentOrder>(101, taskQueue);
        order1->addProductAsync(laptop, 5);
        this_thread::sleep_for(chrono::milliseconds(50));
        order1->addProductAsync(mouse, 10);
        this_thread::sleep_for(chrono::milliseconds(100));
        order1->print();
        orders.push_back(move(order1));
    });
    
    // Заказ 2 - работает в отдельном потоке
    orderThreads.emplace_back([&]() {
        auto order2 = make_unique<ConcurrentOrder>(102, taskQueue);
        order2->addProductAsync(keyboard, 8);
        this_thread::sleep_for(chrono::milliseconds(30));
        order2->addProductAsync(laptop, 3);
        this_thread::sleep_for(chrono::milliseconds(80));
        order2->removeProductAsync(3, 2); // Удаляем 2 клавиатуры
        order2->print();
        orders.push_back(move(order2));
    });
    
    // Заказ 3 - работает в отдельном потоке
    orderThreads.emplace_back([&]() {
        auto order3 = make_unique<ConcurrentOrder>(103, taskQueue);
        order3->checkProductAsync(1, 20); // Проверяем наличие 20 ноутбуков
        order3->addProductAsync(mouse, 15);
        order3->addProductAsync(keyboard, 5);
        this_thread::sleep_for(chrono::milliseconds(70));
        order3->print();
        orders.push_back(move(order3));
    });
    
    // 5. Создаем систему обработки чеков
    ConcurrentReceiptProcessor receiptProcessor(warehouse, 2); // 2 кассира
    
    // Добавляем чеки в обработку
    vector<thread> receiptThreads;
    for (int i = 1; i <= 5; i++) {
        receiptThreads.emplace_back([&, i]() {
            vector<pair<int, int>> items;
            items.push_back({1, 2}); // 2 ноутбука
            items.push_back({2, 3}); // 3 мыши
            receiptProcessor.addReceipt(i, items);
        });
    }
    
    // Ждем завершения всех потоков заказов
    for (auto& thread : orderThreads) {
        thread.join();
    }
    
    // Ждем завершения потоков чеков
    for (auto& thread : receiptThreads) {
        thread.join();
    }
    
    // Даем время на завершение асинхронных операций
    this_thread::sleep_for(chrono::seconds(2));
// 6. Финальное состояние склада
    safePrint("\n=== ФИНАЛЬНОЕ СОСТОЯНИЕ СКЛАДА ===");
    warehouse.printInventory();
    
    // 7. Тест на состояние гонки
    safePrint("\n=== ТЕСТ НА СОСТОЯНИЕ ГОНКИ ===");
    vector<thread> stressThreads;
    
    for (int i = 0; i < 10; i++) {
        stressThreads.emplace_back([&, i]() {
            // 10 потоков одновременно пытаются изменить количество ноутбуков
            if (i % 2 == 0) {
                warehouse.reserveProduct(1, 1);
                safePrint("[СТРЕСС-ТЕСТ] Поток " + to_string(i) + " списал 1 ноутбук");
            } else {
                warehouse.returnProduct(1, 1);
                safePrint("[СТРЕСС-ТЕСТ] Поток " + to_string(i) + " вернул 1 ноутбук");
            }
        });
    }
    
    for (auto& thread : stressThreads) {
        thread.join();
    }
    
    safePrint("\n=== ИТОГОВОЕ КОЛИЧЕСТВО НОУТБУКОВ ===");
    auto finalLaptop = warehouse.findProduct(1);
    if (finalLaptop) {
        safePrint("Ноутбуков на складе: " + to_string(finalLaptop->getQuantity()) + " шт.");
    }
    
    safePrint("\n=== ПРОГРАММА ЗАВЕРШЕНА ===");
    return 0;
}