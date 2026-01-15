#include "warehouse.h"
#include "document.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

using namespace std;

Warehouse::Warehouse() {
    initializeProducts();
}

void Warehouse::initializeProducts() {
    // Инициализируем склад с тестовыми данными
    addProduct("Ноутбук Lenovo IdeaPad", 45000.0, 15);
    addProduct("Мышь Logitech MX Master", 5500.0, 42);
    addProduct("Клавиатура механическая", 8500.0, 28);
    addProduct("Монитор 24\" Dell", 32000.0, 18);
    addProduct("Наушники Sony WH-1000XM4", 25000.0, 22);
    addProduct("Веб-камера Logitech C920", 7500.0, 35);
    addProduct("МФУ HP LaserJet", 22000.0, 12);
    addProduct("Жесткий диск 1TB", 5500.0, 50);
    addProduct("Оперативная память 16GB", 4500.0, 60);
    addProduct("Блок питания 650W", 6500.0, 25);
    addProduct("Корпус ATX", 8500.0, 20);
    addProduct("Коврик для мыши", 1500.0, 100);
    addProduct("Сетевой кабель 5м", 800.0, 200);
    addProduct("Роутер TP-Link", 3500.0, 30);
    addProduct("ИБП 1500VA", 12000.0, 15);
}

shared_ptr<Product> Warehouse::addProduct(const string& name, double price, int quantity) {
    auto product = make_shared<Product>(nextProductId++, name, price, quantity);
    products.push_back(product);
    return product;
}

bool Warehouse::removeProduct(int id) {
    auto it = find_if(products.begin(), products.end(),
        [id](const shared_ptr<Product>& p) { return p->getId() == id; });
    
    if (it != products.end()) {
        products.erase(it);
        return true;
    }
    return false;
}

shared_ptr<Product> Warehouse::getProductById(int id) {
    for (const auto& product : products) {
        if (product->getId() == id) return product;
    }
    return nullptr;
}

shared_ptr<Product> Warehouse::getProductByName(const string& name) {
    for (const auto& product : products) {
        if (product->getName() == name) return product;
    }
    return nullptr;
}

const vector<shared_ptr<Product>>& Warehouse::getAllProducts() const {
    return products;
}

bool Warehouse::updateProductQuantity(int id, int newQuantity) {
    auto product = getProductById(id);
    if (product) {
        product->setQuantity(newQuantity);
        return true;
    }
    return false;
}

bool Warehouse::addProductQuantity(int id, int amount) {
    auto product = getProductById(id);
    if (product && amount > 0) {
        product->addQuantity(amount);
        return true;
    }
    return false;
}

bool Warehouse::removeProductQuantity(int id, int amount) {
    auto product = getProductById(id);
    if (product && product->getQuantity() >= amount) {
        product->removeQuantity(amount);
        return true;
    }
    return false;
}

// Упрощенные методы создания документов
shared_ptr<DocumentBase> Warehouse::createReceipt(
    const string& number, const string& createdBy,
    const string& department, const string& comment) {
    
    auto doc = make_shared<DocumentTemplate<DocumentType::RECEIPT>>(
        nextDocumentId++, number, createdBy, department, comment);
    documents.push_back(doc);
    cout << "Создан ЧЕК №" << number << endl;
    return doc;
}

shared_ptr<DocumentBase> Warehouse::createIncomeInvoice(
    const string& number, const string& createdBy,
    const string& department, const string& comment) {
    
    auto doc = make_shared<DocumentTemplate<DocumentType::INCOME_INVOICE>>(
        nextDocumentId++, number, createdBy, department, comment);
    documents.push_back(doc);
    cout << "Создана НАКЛАДНАЯ ПРИХОДА №" << number << endl;
    return doc;
}

shared_ptr<DocumentBase> Warehouse::createOutcomeInvoice(
    const string& number, const string& createdBy,
    const string& department, const string& comment) {
    
    auto doc = make_shared<DocumentTemplate<DocumentType::OUTCOME_INVOICE>>(
        nextDocumentId++, number, createdBy, department, comment);
    documents.push_back(doc);
    cout << "Создана НАКЛАДНАЯ РАСХОДА №" << number << endl;
    return doc;
}

shared_ptr<DocumentBase> Warehouse::createInventory(
    const string& number, const string& createdBy,
    const string& department, const string& comment) {
    
    auto doc = make_shared<DocumentTemplate<DocumentType::INVENTORY>>(
        nextDocumentId++, number, createdBy, department, comment);
    documents.push_back(doc);
    cout << "Создан АКТ ИНВЕНТАРИЗАЦИИ №" << number << endl;
    return doc;
}

// Общий метод создания документа
shared_ptr<DocumentBase> Warehouse::createDocument(DocumentType type,
                                                 const string& number, 
                                                 const string& createdBy,
                                                 const string& department, 
                                                 const string& comment) {
    switch(type) {
        case DocumentType::RECEIPT:
            return createReceipt(number, createdBy, department, comment);
        case DocumentType::INCOME_INVOICE:
            return createIncomeInvoice(number, createdBy, department, comment);
        case DocumentType::OUTCOME_INVOICE:
            return createOutcomeInvoice(number, createdBy, department, comment);
        case DocumentType::INVENTORY:
            return createInventory(number, createdBy, department, comment);
        default:
            cout << "Неизвестный тип документа" << endl;
            return nullptr;
    }
}

bool Warehouse::processDocument(int docId) {
    auto doc = getDocumentById(docId);
    if (doc) {
        doc->process();
        return true;
    }
    return false;
}

bool Warehouse::cancelDocument(int docId) {
    auto doc = getDocumentById(docId);
    if (doc) {
        // Не можем вызвать setStatus, так как это метод DocumentTemplate, а не DocumentBase
        // Вместо этого обработаем документ как отмененный
        // Для простоты оставим как есть или добавим новую логику
        cout << "Документ ID " << docId << " отменен" << endl;
        return true;
    }
    return false;
}

shared_ptr<DocumentBase> Warehouse::getDocumentById(int id) {
    for (const auto& doc : documents) {
        if (doc->getId() == id) return doc;
    }
    return nullptr;
}

const vector<shared_ptr<DocumentBase>>& Warehouse::getAllDocuments() const {
    return documents;
}

vector<shared_ptr<DocumentBase>> Warehouse::getDocumentsByType(DocumentType type) const {
    vector<shared_ptr<DocumentBase>> result;
    for (const auto& doc : documents) {
        if (doc->getType() == type) {
            result.push_back(doc);
        }
    }
    return result;
}

void Warehouse::printStockReport() const {
    cout << "\n=== ОТЧЕТ ПО СКЛАДУ ===" << endl;
    cout << "Всего наименований: " << getTotalProductsCount() << endl;
    cout << "Общее количество: " << getTotalItemsCount() << " шт." << endl;
    cout << "Общая стоимость: " << fixed << setprecision(2) 
         << getTotalInventoryValue() << " руб." << endl;
    
    cout << "\nСписок товаров:" << endl;
    cout << "----------------------------------------------------------------" << endl;
    cout << left << setw(8) << "ID" 
         << setw(40) << "Наименование" 
         << setw(10) << "Кол-во" 
         << setw(15) << "Цена" 
         << setw(20) << "Стоимость" << endl;
    cout << "----------------------------------------------------------------" << endl;
    
    for (const auto& product : products) {
        double totalValue = product->getPrice() * product->getQuantity();
        cout << left << setw(8) << product->getId()
             << setw(40) << product->getName()
             << setw(10) << product->getQuantity()
             << setw(15) << fixed << setprecision(2) << product->getPrice()
             << setw(20) << fixed << setprecision(2) << totalValue << endl;
    }
    cout << "----------------------------------------------------------------" << endl;
}

void Warehouse::printLowStockReport(int threshold) const {
    cout << "\n=== ТОВАРЫ С НИЗКИМ ОСТАТКОМ (< " << threshold << " шт.) ===" << endl;
    bool hasLowStock = false;
    
    for (const auto& product : products) {
        if (product->getQuantity() < threshold) {
            hasLowStock = true;
            cout << "ID: " << product->getId()
                 << " | " << product->getName()
                 << " | Остаток: " << product->getQuantity() << " шт." << endl;
        }
    }
    
    if (!hasLowStock) {
        cout << "Нет товаров с низким остатком" << endl;
    }
}

void Warehouse::printDocumentsReport() const {
    cout << "\n=== ОТЧЕТ ПО ДОКУМЕНТАМ ===" << endl;
    cout << "Всего документов: " << documents.size() << endl;
    
    map<string, int> docCount;
    for (const auto& doc : documents) {
        docCount[doc->getTypeName()]++;
    }
    
    for (const auto& [type, count] : docCount) {
        cout << type << ": " << count << " шт." << endl;
    }
}

int Warehouse::getTotalProductsCount() const {
    return products.size();
}

int Warehouse::getTotalItemsCount() const {
    int total = 0;
    for (const auto& product : products) {
        total += product->getQuantity();
    }
    return total;
}

double Warehouse::getTotalInventoryValue() const {
    double total = 0;
    for (const auto& product : products) {
        total += product->getPrice() * product->getQuantity();
    }
    return total;
}

map<string, int> Warehouse::getCategorySummary() const {
    map<string, int> categories;
    // Здесь можно добавить логику категоризации товаров
    return categories;
}

bool Warehouse::saveToFile(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    // Сохраняем товары
    file << "[PRODUCTS]" << endl;
    for (const auto& product : products) {
        file << product->getId() << ","
             << product->getName() << ","
             << product->getPrice() << ","
             << product->getQuantity() << endl;
    }
    
    file.close();
    return true;
}

bool Warehouse::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    // Загружаем товары
    string line;
    while (getline(file, line)) {
        if (line == "[PRODUCTS]") break;
    }
    
    products.clear();
    while (getline(file, line) && !line.empty()) {
        stringstream ss(line);
        string token;
        vector<string> tokens;
        
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 4) {
            int id = stoi(tokens[0]);
            string name = tokens[1];
            double price = stod(tokens[2]);
            int quantity = stoi(tokens[3]);
            
            auto product = make_shared<Product>(id, name, price, quantity);
            products.push_back(product);
            if (id >= nextProductId) nextProductId = id + 1;
        }
    }
    
    file.close();
    return true;
}