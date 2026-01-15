#include "warehouse.h"
#include <iostream>

using namespace std;

// Конструктор
Warehouse::Warehouse() {
    // Инициализация тестовыми данными
    addTestData();
}

// Добавление тестовых данных
void Warehouse::addTestData() {
    addProduct(make_shared<Product>(1, "Ноутбук Lenovo", 65000.0));
    addProduct(make_shared<Product>(2, "Компьютерная мышь", 1500.0));
    addProduct(make_shared<Product>(3, "Механическая клавиатура", 4500.0));
    addProduct(make_shared<Product>(4, "Монитор 27\"", 32000.0));
    addProduct(make_shared<Product>(5, "Игровые наушники", 7500.0));
    addProduct(make_shared<Product>(6, "Веб-камера", 3500.0));
    addProduct(make_shared<Product>(7, "Микрофон", 5500.0));
    addProduct(make_shared<Product>(8, "Коврик для мыши", 800.0));
    
    cout << "Склад инициализирован с тестовыми данными" << endl;
}

// Добавление продукта
void Warehouse::addProduct(shared_ptr<Product> product) {
    products.push_back(product);
    cout << "Добавлен товар: " << product->getName() 
         << " (ID: " << product->getId() << ")" << endl;
}

// Получение продукта по ID
shared_ptr<Product> Warehouse::getProductById(int id) {
    for (const auto& product : products) {
        if (product->getId() == id) {
            return product;
        }
    }
    cout << "Товар с ID " << id << " не найден" << endl;
    return nullptr;
}

// Получение всех продуктов
const vector<shared_ptr<Product>>& Warehouse::getAllProducts() const {
    return products;
}

// Создание чека
shared_ptr<DocumentBase> Warehouse::createReceipt(
    const string& number, const string& createdBy, 
    const string& department, const string& comment) {
    
    auto doc = make_shared<DocumentTemplate<DocumentType::RECEIPT>>(
        nextDocumentId++, number, createdBy, department, comment);
    documents.push_back(doc);
    cout << "Создан ЧЕК №" << number << endl;
    return doc;
}

// Создание накладной прихода
shared_ptr<DocumentBase> Warehouse::createIncomeInvoice(
    const string& number, const string& createdBy, 
    const string& department, const string& comment) {
    
    auto doc = make_shared<DocumentTemplate<DocumentType::INCOME_INVOICE>>(
        nextDocumentId++, number, createdBy, department, comment);
    documents.push_back(doc);
    cout << "Создана НАКЛАДНАЯ ПРИХОДА №" << number << endl;
    return doc;
}

// Создание накладной расхода
shared_ptr<DocumentBase> Warehouse::createOutcomeInvoice(
    const string& number, const string& createdBy, 
    const string& department, const string& comment) {
    
    auto doc = make_shared<DocumentTemplate<DocumentType::OUTCOME_INVOICE>>(
        nextDocumentId++, number, createdBy, department, comment);
    documents.push_back(doc);
    cout << "Создана НАКЛАДНАЯ РАСХОДА №" << number << endl;
    return doc;
}

// Создание акта инвентаризации
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

// Получение документа по ID
shared_ptr<DocumentBase> Warehouse::getDocumentById(int id) {
    for (const auto& doc : documents) {
        if (doc->getId() == id) {
            return doc;
        }
    }
    cout << "Документ с ID " << id << " не найден" << endl;
    return nullptr;
}

// Получение всех документов
const vector<shared_ptr<DocumentBase>>& Warehouse::getAllDocuments() const {
    return documents;
}

// Добавление товара в документ
void Warehouse::addProductToDocument(shared_ptr<DocumentBase> document, 
                                    shared_ptr<Product> product, 
                                    int quantity, 
                                    const string& comment) {
    if (document && product) {
        document->addItem(product, quantity, comment);
        cout << "Добавлен товар '" << product->getName() 
             << "' в документ ID: " << document->getId() << endl;
    }
}

// Вывод всех продуктов
void Warehouse::printAllProducts() {
    cout << "\n=== СПИСОК ТОВАРОВ НА СКЛАДЕ ===" << endl;
    cout << "Всего товаров: " << products.size() << endl;
    cout << "--------------------------------" << endl;
    
    for (const auto& product : products) {
        cout << "ID: " << product->getId() 
             << " | Название: " << product->getName()
             << " | Цена: " << product->getPrice() << " руб." << endl;
    }
    cout << "================================\n" << endl;
}

// Вывод всех документов
void Warehouse::printAllDocuments() {
    cout << "\n=== СПИСОК ДОКУМЕНТОВ ===" << endl;
    cout << "Всего документов: " << documents.size() << endl;
    cout << "-------------------------" << endl;
    
    for (const auto& doc : documents) {
        cout << "ID: " << doc->getId()
             << " | Тип: " << doc->getTypeName() 
             << " | №" << doc->getNumber() 
             << " | Статус: " << doc->getStatus() << endl;
    }
    cout << "==========================\n" << endl;
}

// Создание тестового документа
void Warehouse::createTestDocument() {
    cout << "\n=== СОЗДАНИЕ ТЕСТОВОГО ДОКУМЕНТА ===" << endl;
    
    auto doc = createReceipt("ЧК-001", "Иванов И.И.", "Основной склад", "Тестовый чек");
    
    if (!products.empty()) {
        // Добавляем товары в документ
        addProductToDocument(doc, products[0], 2, "Для отдела продаж");
        addProductToDocument(doc, products[1], 5, "Офисные мыши");
        addProductToDocument(doc, products[2], 1, "Для директора");
        
        // Установка специфичных полей
        doc->setSpecificField("Сотрудник", "Петров П.П.");
        doc->setSpecificField("Смена", "Дневная");
        doc->setSpecificField("Номер заказа", "ORD-2024-001");
        doc->setSpecificField("Тип оплаты", "Банковская карта");
        
        // Обработка документа
        doc->process();
        
        // Вывод информации о документе
        doc->print();
    }
    
    cout << "Тестовый документ успешно создан!" << endl;
}