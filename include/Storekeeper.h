#pragma once
#include "Employee.h"
#include <string>

class Storekeeper : public Employee {
private:
    std::string responsibilityZone;
    
public:
    void receiveGoods();
    void shipGoods();
};