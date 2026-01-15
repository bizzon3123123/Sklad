#pragma once
#include <string>

class Customer {
private:
    int id;
    std::string name;
    std::string phone;
    std::string email;
    std::string address;
    
public:
    void registerCustomer();
    void updateData();
};