#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "product.h"
#include "document_type.h"  // Включаем отдельный файл с enum
#include <vector>
#include <memory>
#include <string>
#include <ctime>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>

struct DocumentItem {
    std::shared_ptr<Product> product;
    int quantity;
    std::string comment;
    
    DocumentItem(std::shared_ptr<Product> _product, int _quantity = 1, 
                std::string _comment = "")
        : product(_product), quantity(_quantity), comment(_comment) {}
};

// Базовый класс для всех документов
class DocumentBase {
public:
    virtual ~DocumentBase() = default;
    
    virtual void addItem(std::shared_ptr<Product> product, int quantity = 1, 
                        std::string comment = "") = 0;
    virtual void setSpecificField(const std::string& fieldName, 
                                 const std::string& value) = 0;
    virtual void process() = 0;
    virtual void print() const = 0;
    virtual void saveToFile(const std::string& filename = "") const = 0;
    virtual std::string getTypeName() const = 0;
    virtual int getId() const = 0;
    virtual std::string getNumber() const = 0;
    virtual std::string getStatus() const = 0;
    virtual std::string getComment() const = 0;
    virtual time_t getDate() const = 0;
    virtual DocumentType getType() const = 0;
    virtual std::vector<DocumentItem> getItems() const = 0;
};

template<DocumentType Type>
class DocumentTemplate : public DocumentBase {
protected:
    int id;
    std::string number;
    time_t date;
    std::string createdBy;
    std::string department;
    std::vector<DocumentItem> items;
    std::string status;
    std::string comment;
    std::map<std::string, std::string> specificFields;

public:
    DocumentTemplate(int _id, std::string _number, std::string _createdBy, 
                    std::string _department = "", std::string _comment = "")
        : id(_id), number(_number), createdBy(_createdBy),
          department(_department), comment(_comment) {
        date = time(nullptr);
        status = "Черновик";
        initializeSpecificFields();
    }

    void initializeSpecificFields() {
        switch(Type) {
            case DocumentType::RECEIPT:
                specificFields = {
                    {"Сотрудник", ""},
                    {"Смена", ""},
                    {"Номер заказа", ""},
                    {"Тип оплаты", "Наличные"}
                };
                break;
                
            case DocumentType::INCOME_INVOICE:
                specificFields = {
                    {"Поставщик", ""},
                    {"Договор", ""},
                    {"Счет-фактура", ""},
                    {"Склад приемки", "Основной"}
                };
                break;
                
            case DocumentType::OUTCOME_INVOICE:
                specificFields = {
                    {"Получатель", ""},
                    {"Основание", ""},
                    {"Склад отгрузки", "Основной"},
                    {"Транспорт", ""}
                };
                break;
                
            case DocumentType::INVENTORY:
                specificFields = {
                    {"Комиссия", ""},
                    {"Склад", "Основной"},
                    {"Причина", "Плановая инвентаризация"},
                    {"Результат", "Не проведена"}
                };
                break;
        }
    }

    void addItem(std::shared_ptr<Product> product, int quantity = 1, 
                std::string comment = "") override {
        items.push_back(DocumentItem(product, quantity, comment));
    }

    void setSpecificField(const std::string& fieldName, 
                         const std::string& value) override {
        if (specificFields.find(fieldName) != specificFields.end()) {
            specificFields[fieldName] = value;
        }
    }

    std::string getTypeName() const override {
        switch(Type) {
            case DocumentType::RECEIPT: return "ЧЕК";
            case DocumentType::INCOME_INVOICE: return "НАКЛАДНАЯ ПРИХОДА";
            case DocumentType::OUTCOME_INVOICE: return "НАКЛАДНАЯ РАСХОДА";
            case DocumentType::INVENTORY: return "АКТ ИНВЕНТАРИЗАЦИИ";
            default: return "ДОКУМЕНТ";
        }
    }

    DocumentType getType() const override {
        return Type;
    }

    void print() const override {
        std::cout << "=== " << getTypeName() << " ===" << std::endl;
        std::cout << "Номер: " << number << std::endl;
        
        // Исправляем localtime
        std::tm* tm_info = std::localtime(&date);
        char buffer[26];
        std::strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        std::cout << "Дата: " << buffer << std::endl;
        
        std::cout << "Создал: " << createdBy << std::endl;
        std::cout << "Подразделение: " << department << std::endl;
        std::cout << "Статус: " << status << std::endl;
        
        std::cout << "\nСпецифичные поля:" << std::endl;
        for (const auto& field : specificFields) {
            std::cout << "  " << field.first << ": " << field.second << std::endl;
        }
        
        std::cout << "\nПозиции:" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        for (const auto& item : items) {
            std::cout << item.product->getName() << " × " << item.quantity;
            if (!item.comment.empty()) {
                std::cout << " (" << item.comment << ")";
            }
            std::cout << std::endl;
        }
        std::cout << "----------------------------------------" << std::endl;
    }

    void saveToFile(const std::string& filename = "") const override {
        std::string actualFilename = filename;
        if (actualFilename.empty()) {
            char buffer[80];
            std::tm* tm_info = std::localtime(&date);
            std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", tm_info);
            actualFilename = getTypeName() + "_" + number + "_" + buffer + ".txt";
        }
        
        std::ofstream file(actualFilename);
        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файла: " << actualFilename << std::endl;
            return;
        }
        
        // Сохраняем информацию в файл
        file << "=== " << getTypeName() << " ===" << std::endl;
        file << "Номер: " << number << std::endl;
        
        char dateBuffer[80];
        std::tm* tm_info = std::localtime(&date);
        std::strftime(dateBuffer, sizeof(dateBuffer), "%d.%m.%Y %H:%M:%S", tm_info);
        file << "Дата: " << dateBuffer << std::endl;
        
        file << "Создал: " << createdBy << std::endl;
        file << "Подразделение: " << department << std::endl;
        file << "Статус: " << status << std::endl;
        
        if (!comment.empty()) {
            file << "Комментарий: " << comment << std::endl;
        }
        
        file << "\nСпецифичные поля:" << std::endl;
        for (const auto& field : specificFields) {
            file << "  " << field.first << ": " << field.second << std::endl;
        }
        
        file << "\nПозиции:" << std::endl;
        file << "----------------------------------------" << std::endl;
        double total = 0.0;
        for (const auto& item : items) {
            double itemTotal = item.product->getPrice() * item.quantity;
            total += itemTotal;
            file << item.product->getName() << " × " << item.quantity;
            file << " | Цена: " << item.product->getPrice() << " руб.";
            file << " | Сумма: " << itemTotal << " руб.";
            if (!item.comment.empty()) {
                file << " (" << item.comment << ")";
            }
            file << std::endl;
        }
        file << "----------------------------------------" << std::endl;
        file << "ИТОГО: " << total << " руб." << std::endl;
        
        file.close();
        std::cout << "Документ сохранен в файл: " << actualFilename << std::endl;
    }

    void process() override {
        std::cout << "Обработка " << getTypeName() << " №" << number << std::endl;
        
        switch(Type) {
            case DocumentType::RECEIPT:
                status = "Продано";
                std::cout << "Чек проведен через кассу" << std::endl;
                break;
            case DocumentType::INCOME_INVOICE:
                status = "Принято";
                std::cout << "Товары приняты на склад" << std::endl;
                break;
            case DocumentType::OUTCOME_INVOICE:
                status = "Отгружено";
                std::cout << "Товары отгружены со склада" << std::endl;
                break;
            case DocumentType::INVENTORY:
                status = "Проведена";
                std::cout << "Инвентаризация завершена" << std::endl;
                break;
        }
    }

    int getId() const override { return id; }
    std::string getNumber() const override { return number; }
    std::string getStatus() const override { return status; }
    std::string getComment() const override { return comment; }
    time_t getDate() const override { return date; }
    std::vector<DocumentItem> getItems() const override { return items; }
    
    void setStatus(const std::string& newStatus) { status = newStatus; }
};

#endif // DOCUMENT_H