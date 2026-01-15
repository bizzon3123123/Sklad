#pragma once
#include <string>

class Employee {
protected:
    int id;
    std::string name;
    std::string position;
    std::string shift;
    int accessLevel;
    
public:
    virtual void authenticate();
    virtual void performOperation();
    virtual ~Employee() = default;
};