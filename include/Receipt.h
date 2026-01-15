#pragma once
#include <string>
#include <vector>
#include "Product.h"
#include "Customer.h"
#include "Employee.h"

class Receipt {
private:
    int id;
    std::string creationDate;
    double totalAmount;
    int employeeId;
    int customerId;
    
public:
    void scanProduct();
    void removeProduct();
    void calculateTotal();
    void printReceipt();
};