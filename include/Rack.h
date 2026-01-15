#pragma once
#include <string>

class Product; // предварительное объявление

class Rack {
private:
    int id;
    std::string number;
    std::string section;
    int level;
    int capacity;
    int currentLoad;
    
public:
    void findFreeSpace();
    void getInfo();
};