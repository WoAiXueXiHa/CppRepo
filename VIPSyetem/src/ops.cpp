// ops.cpp
// 各种操作的实现（在子进程中执行），所有 I/O 都通过 request/result 文件完成，避免直接向终端写入。
// 详细说明：
// - 子进程职责：读取由父进程写入的 request 文件，执行对 data 文件的读写（通过 core 提供的 load/save），
//   然后把字符串结果写入 result 文件；子进程不进行终端交互或直接输出。
// - 父进程职责：收集用户输入、写 request 文件、fork 子进程执行操作、等待子进程完成、读取 result 并统一打印。
// - 这样可以保证终端输出由父进程单点控制，避免多进程同时写屏导致输出混淆。

#include "ops.h"
#include "core.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>

using namespace std;

// 读取临时文件（直接复用 core 的带锁读写）
// 说明：临时 request/result 文件也可以被并发访问，因此同样使用 core 中的 read_file/write_file
// 来获得文件锁再进行 I/O。子进程仅通过这些文件与父进程通信，避免直接写终端。
static string read_temp(const string &path) {
    return read_file(path.c_str());
}

// 写临时文件（覆盖）
static void write_temp(const string &path, const string &content) {
    write_file(path.c_str(), content);
}

// 将整数转为字符串（兼容 C++98）
static string int_to_string(int x) { ostringstream ss; ss << x; return ss.str(); }

/**
 * op_list_products
 *  - 读取商品数据并按最终价格排序后写入 result 文件（父进程负责读取并打印）
 *  - request_path 可为空，此函数不需要外部输入
 *  示例 result 内容：
 *  -- 产品列表 --
 *  1 - Mouse (Physical) : $xx + ship $y = $zz
 */
void op_list_products(const string &product_path, const string &request_path, const string &result_path) {
    vector<Product*> products; load_products(product_path, products);
    sort(products.begin(), products.end(), product_cmp);
    ostringstream out;
    out << "-- 产品列表 --\n";
    for (size_t i = 0; i < products.size(); ++i) out << products[i]->info() << "\n";
    for (size_t i = 0; i < products.size(); ++i) delete products[i];
    write_temp(result_path, out.str());
}

// op_add_product
// request 格式：type|name|price|extra
// - type: P 表示实物，D 表示数字
// - name: 非空字符串
// - price: >= 0
// - extra: 实物为邮费，数字商品可为 0
// 说明：子进程在写入 products.txt 时会获得写锁，因此本函数可安全地直接调用 save_products
void op_add_product(const string &product_path, const string &request_path, const string &result_path) {
    vector<Product*> products; load_products(product_path, products);
    string req = read_temp(request_path);
    // 解析 request
    vector<string> parts; istringstream rs(req); string t;
    while (getline(rs, t, '|')) parts.push_back(t);
    int id = 1; if (!products.empty()) { int maxid=0; for (size_t i=0;i<products.size();++i) if (products[i]->getId()>maxid) maxid=products[i]->getId(); id = maxid+1; }
    if (parts.size() >= 3) {
        string type = parts[0]; string name = parts[1]; double price = atof(parts[2].c_str()); double extra = 0.0; if (parts.size()>=4) extra = atof(parts[3].c_str());
        // 基本校验：名称非空，价格非负
        if (name.empty() || price < 0) { write_temp(result_path, string("请求字段非法")); for (size_t i=0;i<products.size();++i) delete products[i]; return; }
        if (type == "P") products.push_back(new PhysicalProduct(id, name, price, extra)); else products.push_back(new DigitalProduct(id, name, price));
        save_products(product_path, products);
        write_temp(result_path, string("添加完成，ID=") + int_to_string(id));
    } else write_temp(result_path, string("请求格式错误"));
    for (size_t i = 0; i < products.size(); ++i) delete products[i];
}

// op_list_users
// 读取用户并将信息写入 result
void op_list_users(const string &user_path, const string &request_path, const string &result_path) {
    vector<User> users; load_users(user_path, users);
    ostringstream out; out << "-- 用户列表 --\n";
    for (size_t i = 0; i < users.size(); ++i) out << users[i].info() << "\n";
    write_temp(result_path, out.str());
}

// op_register_user
// request: name|balance
// 说明：最小校验 name 非空且 balance >= 0，成功则新增用户并返回 ID
void op_register_user(const string &user_path, const string &request_path, const string &result_path) {
    vector<User> users; load_users(user_path, users);
    string req = read_temp(request_path);
    vector<string> parts; istringstream rs(req); string t;
    while (getline(rs, t, '|')) parts.push_back(t);
    int id = 1; if (!users.empty()) { int maxid=0; for (size_t i=0;i<users.size();++i) if (users[i].getId()>maxid) maxid=users[i].getId(); id = maxid+1; }
    if (parts.size() >= 2) {
        string name = parts[0]; double bal = atof(parts[1].c_str());
        if (name.empty() || bal < 0) { write_temp(result_path, string("请求字段非法")); return; }
        users.push_back(User(id, name, bal, false));
        save_users(user_path, users);
        write_temp(result_path, string("注册完成，ID=") + int_to_string(id));
    } else write_temp(result_path, string("请求格式错误"));
}

// op_purchase
// request: uid|pid
// 步骤：查找用户、查找商品、计算最终价格（VIP 折扣），尝试扣款并保存用户数据
void op_purchase(const string &product_path, const string &user_path, const string &request_path, const string &result_path) {
    vector<Product*> products; load_products(product_path, products);
    vector<User> users; load_users(user_path, users);
    string req = read_temp(request_path);
    vector<string> parts; istringstream rs(req); string t;
    while (getline(rs, t, '|')) parts.push_back(t);
    ostringstream out;
    if (parts.size() >= 2) {
        int uid = atoi(parts[0].c_str()); int pid = atoi(parts[1].c_str());
        User* u = NULL; for (size_t i=0;i<users.size();++i) if (users[i].getId()==uid) { u=&users[i]; break; }
        Product* p = NULL; for (size_t i=0;i<products.size();++i) if (products[i]->getId()==pid) { p=products[i]; break; }
        if (!u) out << "未找到用户\n";
        else if (!p) out << "未找到商品\n";
        else {
            double price = p->finalPrice(); if (u->isVIP()) price *= 0.9; // VIP 9 折
            out << "最终价格: $" << price << "\n";
            if (u->charge(price)) { save_users(user_path, users); out << "购买成功，剩余余额 $" << u->getBalance() << "\n"; }
            else out << "余额不足\n";
        }
    } else out << "请求格式错误\n";
    write_temp(result_path, out.str());
    for (size_t i = 0; i < products.size(); ++i) delete products[i];
}

// op_upgrade_vip
// request: uid
void op_upgrade_vip(const string &user_path, const string &request_path, const string &result_path) {
    vector<User> users; load_users(user_path, users);
    string req = read_temp(request_path);
    int uid = atoi(req.c_str());
    ostringstream out;
    User* u = NULL; for (size_t i=0;i<users.size();++i) if (users[i].getId()==uid) { u=&users[i]; break; }
    if (!u) out << "未找到用户\n";
    else {
        const double fee = 20.0;
        if (u->charge(fee)) { u->setVIP(true); save_users(user_path, users); out << "升级成功，现在为 VIP\n"; }
        else out << "余额不足，升级失败\n";
    }
    write_temp(result_path, out.str());
}
