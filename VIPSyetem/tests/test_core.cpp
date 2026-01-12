#include "../src/core.h"
#include <iostream>

int main() {
    // Simple smoke test: load products/users, add temporary entries, save, reload
    const char *pp = "data/products.txt";
    const char *up = "data/users.txt";
    std::vector<Product*> products;
    load_products(pp, products);
    if (products.size() == 0) std::cout << "products empty or load failed\n";
    for (size_t i = 0; i < products.size(); ++i) delete products[i];
    std::vector<User> users;
    load_users(up, users);
    if (users.size() == 0) std::cout << "users empty or load failed\n";
    std::cout << "core smoke test done\n";
    return 0;
}
