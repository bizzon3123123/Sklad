#pragma once
#include "Employee.h"
#include <string>

class Manager : public Employee {
private:
    std::string department;
    std::string accessRights;
    
public:
    void manageUsers();
    void generateReports();
    void manageOrders();
    void workWithSuppliers();
};