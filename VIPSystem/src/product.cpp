#include "../include/product.h"
#include <sstream>
#include <cstdlib>

Product::Product() : Item(), _price(0.0), _stock(0) {}
Product::Product(int id, const std::string &name, double price, int stock)
    : Item(id, name), _price(price), _stock(stock) {}

double Product::getPrice() const { return _price; }
int Product::getStock() const { return _stock; }
void Product::reduceStock(int qty) { if (qty <= _stock) _stock -= qty; }
void Product::increaseStock(int qty) { _stock += qty; }

std::string Product::toCsv() const {
    std::ostringstream oss;
    oss << getId() << "," << getName() << "," << _price << "," << _stock;
    return oss.str();
}

Product Product::fromCsv(const std::string &line) {
    std::istringstream iss(line);
    std::string token;
    int id = 0; std::string name = ""; double price = 0.0; int stock = 0;
    if (std::getline(iss, token, ',')) id = atoi(token.c_str());
    if (std::getline(iss, token, ',')) name = token;
    if (std::getline(iss, token, ',')) price = atof(token.c_str());
    if (std::getline(iss, token, ',')) stock = atoi(token.c_str());
    return Product(id, name, price, stock);
}
