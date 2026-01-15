#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include "product.h"
#include "document_type.h"  // Включаем отдельный файл с enum
#include <vector>
#include <memory>
#include <iostream>
#include <map>

// Предварительное объявление классов
class DocumentBase;
template<DocumentType> class DocumentTemplate;

class Warehouse {
private:
    std::vector<std::shared_ptr<Product>> products;
    std::vector<std::shared_ptr<DocumentBase>> documents;
    int nextProductId = 1001;
    int nextDocumentId = 1;
    
    void initializeProducts();

public:
    Warehouse();
    
    // Управление товарами
    std::shared_ptr<Product> addProduct(const std::string& name, double price, int quantity);
    bool removeProduct(int id);
    std::shared_ptr<Product> getProductById(int id);
    std::shared_ptr<Product> getProductByName(const std::string& name);
    const std::vector<std::shared_ptr<Product>>& getAllProducts() const;
    
    // Управление количеством
    bool updateProductQuantity(int id, int newQuantity);
    bool addProductQuantity(int id, int amount);
    bool removeProductQuantity(int id, int amount);
    
    // Управление документами
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
    
    // Работа с документами
    bool processDocument(int docId);
    bool cancelDocument(int docId);
    std::shared_ptr<DocumentBase> getDocumentById(int id);
    const std::vector<std::shared_ptr<DocumentBase>>& getAllDocuments() const;
    std::vector<std::shared_ptr<DocumentBase>> getDocumentsByType(DocumentType type) const;
    
    // Отчеты
    void printStockReport() const;
    void printLowStockReport(int threshold = 5) const;
    void printDocumentsReport() const;
    
    // Статистика
    int getTotalProductsCount() const;
    int getTotalItemsCount() const;
    double getTotalInventoryValue() const;
    
    // Сохранение/загрузка
    bool saveToFile(const std::string& filename = "warehouse_data.txt") const;
    bool loadFromFile(const std::string& filename = "warehouse_data.txt");
    
    // Вспомогательные методы
    std::map<std::string, int> getCategorySummary() const;
};

#endif // WAREHOUSE_H