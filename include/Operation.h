#pragma once
#include <string>
#include "Employee.h"

class Operation {
private:
    int id;
    std::string type;
    std::string datetime;
    int employeeId;
    std::string description;
    
public:
    void registerOperation();
};