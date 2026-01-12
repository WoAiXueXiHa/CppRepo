#include <iostream>
#include <string>
#include <sstream>
#include "../include/core.h"
#include "../include/user.h"
#include "../include/product.h"

int main() {
    std::string usersFile = "users.csv";
    std::string productsFile = "products.csv";
    std::string transFile = "transactions.csv";

    VIPSystemCore core(usersFile, productsFile, transFile);
    core.loadAll();

    while (true) {
        std::cout << "--- VIP 系统菜单 ---\n";
        std::cout << "1. 列出商品\n2. 列出用户\n3. 添加用户\n4. 添加商品\n5. 购买商品\n6. 查询用户交易\n7. 保存并退出\n";
        std::cout << "请输入选项: ";
        int opt = 0; if (!(std::cin >> opt)) break;
        if (opt == 1) {
            const std::vector<Product> &ps = core.listProducts();
            for (size_t i = 0; i < ps.size(); ++i) {
                std::cout << ps[i].toCsv() << "\n";
            }
        } else if (opt == 2) {
            const std::vector<User> &us = core.listUsers();
            for (size_t i = 0; i < us.size(); ++i) std::cout << us[i].toCsv() << "\n";
        } else if (opt == 3) {
            std::string name; int vip; int pts;
            std::cout << "姓名: "; std::cin >> name;
            std::cout << "VIP 等级 (整数): "; std::cin >> vip;
            std::cout << "初始积分: "; std::cin >> pts;
            int id = core.listUsers().empty() ? 1 : core.listUsers().back().getId() + 1;
            core.addUser(User(id, name, vip, pts));
            std::cout << "添加用户完成，id=" << id << "\n";
        } else if (opt == 4) {
            std::string name; double price; int stock;
            std::cout << "商品名: "; std::cin >> name;
            std::cout << "价格: "; std::cin >> price;
            std::cout << "库存: "; std::cin >> stock;
            int id = core.listProducts().empty() ? 1 : core.listProducts().back().getId() + 1;
            core.addProduct(Product(id, name, price, stock));
            std::cout << "添加商品完成，id=" << id << "\n";
        } else if (opt == 5) {
            int uid, pid, qty;
            std::cout << "用户 id: "; std::cin >> uid;
            std::cout << "商品 id: "; std::cin >> pid;
            std::cout << "数量: "; std::cin >> qty;
            if (core.makePurchase(uid, pid, qty)) std::cout << "购买成功\n";
            else std::cout << "购买失败（用户/商品不存在 或 库存不足）\n";
        } else if (opt == 6) {
            int uid; std::cout << "用户 id: "; std::cin >> uid;
            std::vector<Transaction> ts = core.queryTransactionsByUser(uid);
            for (size_t i = 0; i < ts.size(); ++i) std::cout << ts[i].toCsv() << "\n";
        } else if (opt == 7) {
            core.saveAll();
            std::cout << "保存完成，退出。\n";
            break;
        } else {
            std::cout << "无效选项\n";
        }
    }
    return 0;
}
