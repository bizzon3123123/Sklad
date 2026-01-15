#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

class Product {
private:
    int id;
    std::string name;
    double price;

public:
    Product(int _id, std::string _name, double _price) 
        : id(_id), name(_name), price(_price) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
};

#endif // PRODUCT_H