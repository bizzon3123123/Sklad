#pragma once
#include <string>
#include <vector>
#include "Product.h"
#include "Supplier.h"

class ProductOrder {
private:
    int id;
    std::string orderDate;
    std::string expectedDate;
    std::string status;
    double totalCost;
    int supplierId;
    
public:
    void createOrder();
    void updateStatus();
    void calculateCost();
    void receiveOrder();
};