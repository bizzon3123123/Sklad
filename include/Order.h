#pragma once
#include <string>
#include <vector>
#include "Product.h"
#include "Customer.h"
#include "Employee.h"

class Order {
private:
    int id;
    std::string creationDate;
    std::string status;
    double totalCost;
    
public:
    void createOrder();
    void changeStatus();
    void calculateCost();
};