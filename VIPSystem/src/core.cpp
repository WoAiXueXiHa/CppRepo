#include "../include/core.h"
#include "../include/c_db_os.h"
#include <sstream>
#include <algorithm>
#include <cstdlib>

VIPSystemCore::VIPSystemCore(const std::string &usersPath,
                             const std::string &productsPath,
                             const std::string &transPath)
    : _usersPath(usersPath), _productsPath(productsPath), _transPath(transPath) {}

// Helper to get next ids (简单策略：在内存中找最大 + 1)
int VIPSystemCore::nextUserId() const {
    int m = 0;
    for (size_t i = 0; i < _users.size(); ++i) if (_users[i].getId() > m) m = _users[i].getId();
    return m + 1;
}
int VIPSystemCore::nextProductId() const {
    int m = 0;
    for (size_t i = 0; i < _products.size(); ++i) if (_products[i].getId() > m) m = _products[i].getId();
    return m + 1;
}
int VIPSystemCore::nextTransactionId() const {
    int m = 0;
    for (size_t i = 0; i < _transactions.size(); ++i) if (_transactions[i].getId() > m) m = _transactions[i].getId();
    return m + 1;
}

bool VIPSystemCore::loadAll() {
    _users.clear(); _products.clear(); _transactions.clear();
    // users
    {
        char *cbuf = NULL; size_t len = 0;
        if (dbos_read_entire_file(_usersPath.c_str(), &cbuf, &len)) {
            std::string buf(cbuf, len);
            free(cbuf);
            std::istringstream iss(buf);
            std::string line;
            while (std::getline(iss, line)) {
                if (line.empty()) continue;
                _users.push_back(User::fromCsv(line));
            }
        }
    }
    // products
    {
        char *cbuf = NULL; size_t len = 0;
        if (dbos_read_entire_file(_productsPath.c_str(), &cbuf, &len)) {
            std::string buf(cbuf, len);
            free(cbuf);
            std::istringstream iss(buf);
            std::string line;
            while (std::getline(iss, line)) {
                if (line.empty()) continue;
                _products.push_back(Product::fromCsv(line));
            }
        }
    }
    // transactions
    {
        char *cbuf = NULL; size_t len = 0;
        if (dbos_read_entire_file(_transPath.c_str(), &cbuf, &len)) {
            std::string buf(cbuf, len);
            free(cbuf);
            std::istringstream iss(buf);
            std::string line;
            while (std::getline(iss, line)) {
                if (line.empty()) continue;
                _transactions.push_back(Transaction::fromCsv(line));
            }
        }
    }
    return true;
}

bool VIPSystemCore::saveAll() {
    // users
    std::ostringstream us;
    for (size_t i = 0; i < _users.size(); ++i) {
        us << _users[i].toCsv() << "\n";
    }
    if (!dbos_write_entire_file(_usersPath.c_str(), us.str().c_str(), us.str().size())) return false;

    // products
    std::ostringstream ps;
    for (size_t i = 0; i < _products.size(); ++i) {
        ps << _products[i].toCsv() << "\n";
    }
    if (!dbos_write_entire_file(_productsPath.c_str(), ps.str().c_str(), ps.str().size())) return false;

    // transactions
    std::ostringstream ts;
    for (size_t i = 0; i < _transactions.size(); ++i) {
        ts << _transactions[i].toCsv() << "\n";
    }
    if (!dbos_write_entire_file(_transPath.c_str(), ts.str().c_str(), ts.str().size())) return false;

    return true;
}


bool VIPSystemCore::addUser(const User &u) {
    _users.push_back(u);
    return true;
}

User *VIPSystemCore::findUserById(int id) {
    for (size_t i = 0; i < _users.size(); ++i) {
        if (_users[i].getId() == id) return &_users[i];
    }
    return NULL;
}

bool VIPSystemCore::addProduct(const Product &p) {
    _products.push_back(p);
    return true;
}

Product *VIPSystemCore::findProductById(int id) {
    for (size_t i = 0; i < _products.size(); ++i) {
        if (_products[i].getId() == id) return &_products[i];
    }
    return NULL;
}

bool VIPSystemCore::makePurchase(int userId, int productId, int qty) {
    User *u = findUserById(userId);
    Product *p = findProductById(productId);
    if (!u || !p) return false;
    if (p->getStock() < qty) return false;

    double total = p->getPrice() * qty;
    p->reduceStock(qty);
    // 简单积分策略：每消费 1 元积 1 分
    u->addPoints((int)total);
    int tid = nextTransactionId();
    Transaction t(tid, userId, productId, qty, total);
    _transactions.push_back(t);
    return true;
}

std::vector<Transaction> VIPSystemCore::queryTransactionsByUser(int userId) const {
    std::vector<Transaction> out;
    for (size_t i = 0; i < _transactions.size(); ++i) {
        if (_transactions[i].getUserId() == userId) out.push_back(_transactions[i]);
    }
    return out;
}

const std::vector<User> &VIPSystemCore::listUsers() const { return _users; }
const std::vector<Product> &VIPSystemCore::listProducts() const { return _products; }
const std::vector<Transaction> &VIPSystemCore::listTransactions() const { return _transactions; }
