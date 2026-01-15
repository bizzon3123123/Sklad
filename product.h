#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

class Product {
private:
    int id;
    std::string name;
    double price;
    int quantity;  // Количество на складе

public:
    Product(int _id, std::string _name, double _price, int _quantity = 0) 
        : id(_id), name(_name), price(_price), quantity(_quantity) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    void setQuantity(int qty) { quantity = qty; }
    void addQuantity(int amount) { quantity += amount; }
    void removeQuantity(int amount) { 
        if (quantity >= amount) quantity -= amount; 
    }
};

#endif // PRODUCT_H