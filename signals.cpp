#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>

using namespace std;


// Класс Товар
class Product {
private:
    int id;
    string name;
    double price;
    int quantity;  // Добавим количество на складе

public:
    Product(int _id, string _name, double _price, int _quantity = 0) 
        : id(_id), name(_name), price(_price), quantity(_quantity) {}

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    void setQuantity(int qty) { quantity = qty; }
    
    // Операторы из предыдущих заданий
    Product& operator++() {
        quantity++;
        return *this;
    }
    
    Product& operator--() {
        if (quantity > 0) quantity--;
        return *this;
    }
};

// Класс Позиция в чеке (из задания с чеками)
class ReceiptItem {
private:
    shared_ptr<Product> product;
    int quantity;

public:
    ReceiptItem(shared_ptr<Product> _product, int _quantity = 1)
        : product(_product), quantity(_quantity) {}

    // Операторы из предыдущих заданий
    ReceiptItem& operator++() {
        quantity++;
        return *this;
    }

    ReceiptItem operator++(int) {
        ReceiptItem temp = *this;
        quantity++;
        return temp;
    }

    ReceiptItem& operator--() {
        if (quantity > 0) {
            quantity--;
        }
        return *this;
    }

    ReceiptItem operator--(int) {
        ReceiptItem temp = *this;
        if (quantity > 0) {
            quantity--;
        }
        return temp;
    }

    shared_ptr<Product> getProduct() const { return product; }
    int getQuantity() const { return quantity; }
    bool isEmpty() const { return quantity == 0; }
};

// Класс Чек (упрощенная версия из предыдущих заданий)
class Receipt {
private:
    int id;
    vector<shared_ptr<ReceiptItem>> items;

public:
    Receipt(int _id) : id(_id) {}

    // Методы из предыдущих заданий
    void addProduct(shared_ptr<Product> product) {
        for (auto& item : items) {
            if (item->getProduct()->getId() == product->getId()) {
                ++(*item);
                return;
            }
        }
        items.push_back(make_shared<ReceiptItem>(product, 1));
    }

    void removeProduct(shared_ptr<Product> product) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if ((*it)->getProduct()->getId() == product->getId()) {
                --(**it);
                if ((*it)->isEmpty()) {
                    items.erase(it);
                }
                break;
            }
        }
    }

    vector<shared_ptr<ReceiptItem>> getItems() const { return items; }
    int getId() const { return id; }
};

// Класс Склад (из UML схемы)
class Warehouse {
private:
    int id;
    string name;
    vector<shared_ptr<Product>> products;

public:
    Warehouse(int _id, string _name) : id(_id), name(_name) {}

    void addProduct(shared_ptr<Product> product) {
        products.push_back(product);
    }

    shared_ptr<Product> findProduct(int productId) {
        for (auto& product : products) {
            if (product->getId() == productId) {
                return product;
            }
        }
        return nullptr;
    }

    void printInventory() {
        cout << "\n=== ИНВЕНТАРЬ СКЛАДА '" << name << "' ===" << endl;
        for (const auto& product : products) {
            cout << product->getName() << ": " << product->getQuantity() << " шт." << endl;
        }
    }
};

// Класс Заказ (из UML схемы)
class Order {
private:
    int id;
    string status;
    double totalCost;
    vector<pair<shared_ptr<Product>, int>> items;

public:
    Order(int _id) : id(_id), status("В обработке"), totalCost(0) {}

    // Добавим связь со складом через параметр
    void addProduct(shared_ptr<Product> product, int quantity, Warehouse& warehouse) {
        // Проверяем наличие на складе
auto warehouseProduct = warehouse.findProduct(product->getId());
        if (warehouseProduct && warehouseProduct->getQuantity() >= quantity) {
            // Уменьшаем количество на складе
            for (int i = 0; i < quantity; i++) {
                --(*warehouseProduct);
            }
            
            // Добавляем в заказ
            items.push_back({product, quantity});
            totalCost += product->getPrice() * quantity;
            cout << "Добавлен товар: " << product->getName() 
                 << " × " << quantity << endl;
        } else {
            cout << "Ошибка: Недостаточно товара '" << product->getName() 
                 << "' на складе!" << endl;
        }
    }

    void print() {
        cout << "\n=== ЗАКАЗ №" << id << " ===" << endl;
        cout << "Статус: " << status << endl;
        cout << "Позиции:" << endl;
        for (const auto& item : items) {
            cout << "  " << item.first->getName() << " × " << item.second << endl;
        }
        cout << "Итого: " << totalCost << " руб." << endl;
    }
};


// Простая система сигналов 
class InventorySignal {
private:
    static InventorySignal* instance;
    map<string, vector<function<void(int, int, string)>>> signals;
    
    InventorySignal() {} // Приватный конструктор
    
public:
    static InventorySignal& get() {
        if (!instance) {
            instance = new InventorySignal();
        }
        return *instance;
    }
    
    // Подписаться на событие изменения количества товара
    void connect(const string& signalName, function<void(int, int, string)> slot) {
        signals[signalName].push_back(slot);
    }
    
    // Отправить сигнал
    void emit(const string& signalName, int productId, int quantity, string action) {
        if (signals.find(signalName) != signals.end()) {
            for (auto& slot : signals[signalName]) {
                slot(productId, quantity, action);
            }
        }
    }
};

InventorySignal* InventorySignal::instance = nullptr;


// Расширенный класс Склад с поддержкой сигналов
class WarehouseWithSignals : public Warehouse {
private:
    int warehouseId;
    
public:
    WarehouseWithSignals(int _id, string _name) : Warehouse(_id, _name), warehouseId(_id) {
        // Подписываемся на сигналы при создании склада
        auto& signalSystem = InventorySignal::get();
        
        // Когда товар добавляется в заказ
        signalSystem.connect("product_ordered", 
            [this](int productId, int quantity, string action) {
                this->onProductOrdered(productId, quantity, action);
            });
            
        // Когда товар удаляется из заказа
        signalSystem.connect("product_removed_from_order",
            [this](int productId, int quantity, string action) {
                this->onProductRemovedFromOrder(productId, quantity, action);
            });
    }
    
    void onProductOrdered(int productId, int quantity, string action) {
        auto product = findProduct(productId);
        if (product) {
            if (product->getQuantity() >= quantity) {
                // Уменьшаем количество
                for (int i = 0; i < quantity; i++) {
                    --(*product);
                }
                cout << "[СИГНАЛ] Со склада списан товар ID " << productId 
                     << " в количестве " << quantity << " шт." << endl;
            }
        }
    }
    
    void onProductRemovedFromOrder(int productId, int quantity, string action) {
        auto product = findProduct(productId);
        if (product) {
            // Увеличиваем количество
            for (int i = 0; i < quantity; i++) {
                ++(*product);
            }
            cout << "[СИГНАЛ] На склад возвращен товар ID " << productId 
                 << " в количестве " << quantity << " шт." << endl;
        }
    }
};
// Расширенный класс Заказ с отправкой сигналов
class OrderWithSignals : public Order {
private:
    int orderId;
    
public:
    OrderWithSignals(int _id) : Order(_id), orderId(_id) {}
    
    void addProductWithSignal(shared_ptr<Product> product, int quantity, Warehouse& warehouse) {
        auto& signalSystem = InventorySignal::get();
        
        // Проверяем наличие
        auto warehouseProduct = warehouse.findProduct(product->getId());
        if (warehouseProduct && warehouseProduct->getQuantity() >= quantity) {
            // Отправляем сигнал перед списанием
            signalSystem.emit("product_ordered", product->getId(), quantity, 
                            "Добавление в заказ №" + to_string(orderId));
            
            // Вызываем родительский метод
            Order::addProduct(product, quantity, warehouse);
        } else {
            cout << "Недостаточно товара на складе!" << endl;
        }
    }
    
    void removeProductWithSignal(int productId, int quantity, Warehouse& warehouse) {
        // Ищем товар в заказе
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->first->getId() == productId) {
                int removeQty = min(quantity, it->second);
                
                // Отправляем сигнал о возврате
                auto& signalSystem = InventorySignal::get();
                signalSystem.emit("product_removed_from_order", productId, removeQty,
                                "Удаление из заказа №" + to_string(orderId));
                
                // Обновляем заказ
                if (it->second > removeQty) {
                    it->second -= removeQty;
                } else {
                    items.erase(it);
                }
                break;
            }
        }
    }
};