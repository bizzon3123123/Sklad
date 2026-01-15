#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include "product.h"
#include "document.h"  // Включаем document.h ПЕРЕД использованием DocumentType
#include <vector>
#include <memory>
#include <iostream>

class Warehouse {
private:
    std::vector<std::shared_ptr<Product>> products;
    std::vector<std::shared_ptr<DocumentBase>> documents;
    int nextDocumentId = 1;
    
    void addTestData();

public:
    Warehouse();
    
    // Управление товарами
    void addProduct(std::shared_ptr<Product> product);
    std::shared_ptr<Product> getProductById(int id);
    const std::vector<std::shared_ptr<Product>>& getAllProducts() const;
    
    // Управление документами - упрощенный вариант
    std::shared_ptr<DocumentBase> createReceipt(
        const std::string& number, const std::string& createdBy,
        const std::string& department = "", const std::string& comment = "");
    
    std::shared_ptr<DocumentBase> createIncomeInvoice(
        const std::string& number, const std::string& createdBy,
        const std::string& department = "", const std::string& comment = "");
    
    std::shared_ptr<DocumentBase> createOutcomeInvoice(
        const std::string& number, const std::string& createdBy,
        const std::string& department = "", const std::string& comment = "");
    
    std::shared_ptr<DocumentBase> createInventory(
        const std::string& number, const std::string& createdBy,
        const std::string& department = "", const std::string& comment = "");
    
    // Общий метод для создания любого документа
    std::shared_ptr<DocumentBase> createDocument(DocumentType type,
                                                const std::string& number, 
                                                const std::string& createdBy,
                                                const std::string& department = "", 
                                                const std::string& comment = "");
    
    std::shared_ptr<DocumentBase> getDocumentById(int id);
    const std::vector<std::shared_ptr<DocumentBase>>& getAllDocuments() const;
    
    // Вспомогательные методы
    void printAllProducts();
    void printAllDocuments();
    void createTestDocument();
    
    // Добавление товара в документ
    void addProductToDocument(std::shared_ptr<DocumentBase> document, 
                             std::shared_ptr<Product> product, 
                             int quantity = 1, 
                             const std::string& comment = "");
};

#endif // WAREHOUSE_H