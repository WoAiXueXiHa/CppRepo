#include "../include/transaction.h"
#include <sstream>
#include <cstdlib>

Transaction::Transaction() : Record(), _userId(0), _productId(0), _qty(0), _total(0.0) {}
Transaction::Transaction(int id, int userId, int productId, int qty, double total)
    : Record(id), _userId(userId), _productId(productId), _qty(qty), _total(total) {}

int Transaction::getUserId() const { return _userId; }
int Transaction::getProductId() const { return _productId; }
int Transaction::getQty() const { return _qty; }
double Transaction::getTotal() const { return _total; }

std::string Transaction::toCsv() const {
    std::ostringstream oss;
    oss << getId() << "," << _userId << "," << _productId << "," << _qty << "," << _total;
    return oss.str();
}

Transaction Transaction::fromCsv(const std::string &line) {
    std::istringstream iss(line);
    std::string token;
    int id = 0, uid = 0, pid = 0, qty = 0; double total = 0.0;
    if (std::getline(iss, token, ',')) id = atoi(token.c_str());
    if (std::getline(iss, token, ',')) uid = atoi(token.c_str());
    if (std::getline(iss, token, ',')) pid = atoi(token.c_str());
    if (std::getline(iss, token, ',')) qty = atoi(token.c_str());
    if (std::getline(iss, token, ',')) total = atof(token.c_str());
    return Transaction(id, uid, pid, qty, total);
}
