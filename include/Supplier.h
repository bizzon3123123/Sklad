#pragma once
#include <string>

class Supplier {
private:
    int id;
    std::string name;
    std::string contacts;
    double rating;
    
public:
    void addSupplier();
    void updateRating();
};