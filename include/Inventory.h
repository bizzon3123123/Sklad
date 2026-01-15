#pragma once
#include <string>

class Inventory {
private:
    int id;
    std::string date;
    std::string result;
    int discrepancies;
    
public:
    void startInventory();
    void completeInventory();
};