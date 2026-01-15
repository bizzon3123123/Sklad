#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <ctime>
#include <map>

using namespace std;


class Product {
private:
    int id;
    string name;
    double price;

public:
    Product(int _id, string _name, double _price) 
        : id(_id), name(_name), price(_price) {}

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
};

enum class DocumentType {
    RECEIPT,           
    INCOME_INVOICE,    
    OUTCOME_INVOICE,   
    INVENTORY          
};


struct DocumentItem {
    shared_ptr<Product> product;
    int quantity;
    string comment;
    
    DocumentItem(shared_ptr<Product> _product, int _quantity = 1, string _comment = "")
        : product(_product), quantity(_quantity), comment(_comment) {}
};


template<DocumentType Type>
class DocumentTemplate {
protected:
    
    int id;
    string number;
    time_t date;
    string createdBy;
    string department;
    vector<DocumentItem> items;
    string status;
    string comment;
    
    
    map<string, string> specificFields;

public:
    DocumentTemplate(int _id, string _number, string _createdBy, 
                    string _department = "", string _comment = "")
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

    
    void addItem(shared_ptr<Product> product, int quantity = 1, string comment = "") {
        items.push_back(DocumentItem(product, quantity, comment));
    }

    
    void setSpecificField(const string& fieldName, const string& value) {
        if (specificFields.find(fieldName) != specificFields.end()) {
            specificFields[fieldName] = value;
        }
    }

    
    string getTypeName() const {
        switch(Type) {
            case DocumentType::RECEIPT: return "ЧЕК";
            case DocumentType::INCOME_INVOICE: return "НАКЛАДНАЯ ПРИХОДА";
            case DocumentType::OUTCOME_INVOICE: return "НАКЛАДНАЯ РАСХОДА";
            case DocumentType::INVENTORY: return "АКТ ИНВЕНТАРИЗАЦИИ";
            default: return "ДОКУМЕНТ";
        }
    }
virtual void print() const {
        cout << "=== " << getTypeName() << " ===" << endl;
        cout << "Номер: " << number << endl;
        cout << "Дата: " << ctime(&date);
        cout << "Создал: " << createdBy << endl;
        cout << "Подразделение: " << department << endl;
        cout << "Статус: " << status << endl;
        
      
        cout << "\nСпецифичные поля:" << endl;
        for (const auto& field : specificFields) {
            cout << "  " << field.first << ": " << field.second << endl;
        }
        
        cout << "\nПозиции:" << endl;
        cout << "----------------------------------------" << endl;
        for (const auto& item : items) {
            cout << item.product->getName() << " × " << item.quantity;
            if (!item.comment.empty()) {
                cout << " (" << item.comment << ")";
            }
            cout << endl;
        }
        cout << "----------------------------------------" << endl;
    }
    virtual void process() {
        cout << "Обработка " << getTypeName() << " №" << number << endl;
        
        switch(Type) {
            case DocumentType::RECEIPT:
                status = "Продано";
                cout << "Чек проведен через кассу" << endl;
                break;
            case DocumentType::INCOME_INVOICE:
                status = "Принято";
                cout << "Товары приняты на склад" << endl;
                break;
            case DocumentType::OUTCOME_INVOICE:
                status = "Отгружено";
                cout << "Товары отгружены со склада" << endl;
                break;
            case DocumentType::INVENTORY:
                status = "Проведена";
                cout << "Инвентаризация завершена" << endl;
                break;
        }
    }

    int getId() const { return id; }
    string getNumber() const { return number; }
    string getStatus() const { return status; }
    void setStatus(const string& newStatus) { status = newStatus; }
};


int main() {

}