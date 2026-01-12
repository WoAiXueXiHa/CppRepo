#include "core.h"
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

using namespace std;

// -------------------------
// Product implementations
// -------------------------
Product::Product(int id_, const string &name_, double price_): _id(id_), _name(name_), _price(price_) {}
Product::~Product() {}
int Product::getId() const { return _id; }
string Product::getName() const { return _name; }
double Product::getPrice() const { return _price; }
double Product::finalPrice() const { return _price; }
string Product::serialize() const { ostringstream ss; ss << _id << "|Base|" << _name << "|" << _price << "|0"; return ss.str(); }
string Product::info() const { ostringstream ss; ss << _id << " - " << _name << " : $" << finalPrice(); return ss.str(); }

PhysicalProduct::PhysicalProduct(int id_, const string &name_, double price_, double shipping_)
    : Product(id_, name_, price_), _shipping(shipping_) {}
double PhysicalProduct::finalPrice() const { return getPrice() + _shipping; }
string PhysicalProduct::serialize() const { ostringstream ss; ss << getId() << "|P|" << getName() << "|" << getPrice() << "|" << _shipping; return ss.str(); }
string PhysicalProduct::info() const { ostringstream ss; ss << getId() << " - " << getName() << " (Physical) : $" << getPrice() << " + ship $" << _shipping << " = $" << finalPrice(); return ss.str(); }

DigitalProduct::DigitalProduct(int id_, const string &name_, double price_): Product(id_, name_, price_) {}
double DigitalProduct::finalPrice() const { return getPrice(); }
string DigitalProduct::serialize() const { ostringstream ss; ss << getId() << "|D|" << getName() << "|" << getPrice() << "|0"; return ss.str(); }
string DigitalProduct::info() const { ostringstream ss; ss << getId() << " - " << getName() << " (Digital) : $" << getPrice(); return ss.str(); }

// -------------------------
// User implementations
// -------------------------
User::User(): _id(0), _name(""), _balance(0.0), _is_vip(false) {}
User::User(int id_, const string &name_, double balance_, bool vip_): _id(id_), _name(name_), _balance(balance_), _is_vip(vip_) {}
int User::getId() const { return _id; }
string User::getName() const { return _name; }
double User::getBalance() const { return _balance; }
bool User::isVIP() const { return _is_vip; }
void User::setVIP(bool v) { _is_vip = v; }
void User::addBalance(double x) { _balance += x; }
bool User::charge(double amount) { if (_balance + 1e-9 >= amount) { _balance -= amount; return true; } return false; }
string User::serialize() const { ostringstream ss; ss << _id << "|" << _name << "|" << _balance << "|" << (_is_vip?1:0); return ss.str(); }
string User::info() const { ostringstream ss; ss << _id << " - " << _name << " : $" << _balance << (_is_vip?" (VIP)":""); return ss.str(); }

// -------------------------
// 文件读写与加锁实现（使用 POSIX 系统调用）
// 详细说明（中文）：
// 1) 目的：在多进程并发访问同一数据文件（如 products.txt / users.txt）时，
//    通过 fcntl 文件锁保证读/写的一致性与互斥，避免出现并发写入导致的文件损坏或不完整数据。
// 2) 锁类型：本实现对整个文件加锁（l_start=0, l_len=0）：
//    - 读操作使用 `F_RDLCK`（共享锁），允许多个读者并发。
//    - 写操作使用 `F_WRLCK`（排他锁），在写入期间阻止其他读/写操作。
// 3) 阻塞行为：使用 `F_SETLKW`（阻塞式获取锁），调用将会等待直到锁可用或者发生错误。
//    优点：逻辑简单，避免复杂的重试逻辑；缺点：可能导致等待时间较长。可根据需要改为非阻塞+重试。
// 4) 写策略：`write_file` 在获得写锁后先 `ftruncate` 将文件清空，再写入全部内容，最后释放锁并关闭文件。
//    这样可以减少读到半成品数据的风险。若希望更高的原子性，可采用写入临时文件后 `rename` 覆盖目标文件。
// 5) 注意事项：持锁期间应尽量减少工作量（不要执行长时间阻塞操作），以避免阻塞其他并发进程。

// apply_lock: 在 fd 上设置锁（F_RDLCK/F_WRLCK/F_UNLCK），阻塞直到成功或出错
static bool apply_lock(int fd, short type) {
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = type; // F_RDLCK 或 F_WRLCK 或 F_UNLCK
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;   // 从文件头开始
    fl.l_len = 0;     // 长度 0 表示到文件末尾（整文件锁）
    if (fcntl(fd, F_SETLKW, &fl) == -1) return false;
    return true;
}

// read_file: 以只读方式打开并加读锁，读取全部内容后解锁并返回字符串
/**
 * read_file: 以只读方式打开并加读锁，读取全部内容后解锁并返回字符串
 * 参数：
 *  - path: 文件路径（const char*）
 * 返回值：文件全部内容的 std::string；若打开或加锁失败，返回空字符串（调用者需据此判定）
 * 注意：此函数阻塞直到获得读锁并读取完成；若需要非阻塞行为，可修改为非阻塞或带超时重试。
 */
string read_file(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return string();
    if (!apply_lock(fd, F_RDLCK)) { close(fd); return string(); }
    string out;
    char buf[1024];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) out.append(buf, buf + n);
    // 释放锁并关闭
    apply_lock(fd, F_UNLCK);
    close(fd);
    return out;
}

// write_file: 打开文件并加写锁，先清空文件再写入全部内容，最后解锁并关闭
/**
 * write_file: 打开文件并加写锁，先清空文件再写入全部内容，最后解锁并关闭
 * 参数：
 *  - path: 文件路径
 *  - content: 要写入的字符串内容（覆盖写入）
 * 返回：成功返回 true；失败返回 false（例如打开失败、加锁失败或写入失败）
 * 实现要点：
 *  - 先获得写锁，确保没有其他读/写并发进行；
 *  - 使用 ftruncate 清空文件，再执行循环 write 直到全部写入完成；
 *  - 写入完成后释放锁并关闭文件。
 * 可能改进：先写入临时文件然后 rename 到目标文件，以提高原子性并避免读到半成品。
 */
bool write_file(const char *path, const string &content) {
    int fd = open(path, O_WRONLY | O_CREAT, 0644);
    if (fd < 0) return false;
    if (!apply_lock(fd, F_WRLCK)) { close(fd); return false; }
    // 清空文件到 0 长度
    if (ftruncate(fd, 0) == -1) { apply_lock(fd, F_UNLCK); close(fd); return false; }
    const char *p = content.c_str();
    ssize_t left = (ssize_t)content.size();
    while (left > 0) {
        ssize_t w = write(fd, p, left);
        if (w <= 0) { apply_lock(fd, F_UNLCK); close(fd); return false; }
        left -= w; p += w;
    }
    // 释放锁并关闭
    apply_lock(fd, F_UNLCK);
    close(fd);
    return true;
}

void load_products(const string &path, vector<Product*> &out) {
    out.clear();
    string content = read_file(path.c_str());
    if (content.empty()) return;
    istringstream ss(content);
    string line;
    while (getline(ss, line)) {
        if (line.size() == 0) continue;
        if (line[0] == '#') continue;
        vector<string> parts;
        istringstream ls(line);
        string token;
        while (getline(ls, token, '|')) parts.push_back(token);
        if (parts.size() < 5) continue;
        int id = atoi(parts[0].c_str());
        string type = parts[1];
        string name = parts[2];
        double price = atof(parts[3].c_str());
        double extra = atof(parts[4].c_str());
        if (type == "P") out.push_back(new PhysicalProduct(id, name, price, extra));
        else out.push_back(new DigitalProduct(id, name, price));
    }
}

void save_products(const string &path, const vector<Product*> &v) {
    ostringstream ss;
    ss << "# id|type|name|price|extra\n";
    for (size_t i = 0; i < v.size(); ++i) ss << v[i]->serialize() << "\n";
    write_file(path.c_str(), ss.str());
}

void load_users(const string &path, vector<User> &out) {
    out.clear();
    string content = read_file(path.c_str());
    if (content.empty()) return;
    istringstream ss(content);
    string line;
    while (getline(ss, line)) {
        if (line.size() == 0) continue;
        if (line[0] == '#') continue;
        vector<string> parts;
        istringstream ls(line);
        string token;
        while (getline(ls, token, '|')) parts.push_back(token);
        if (parts.size() < 4) continue;
        int id = atoi(parts[0].c_str());
        string name = parts[1];
        double bal = atof(parts[2].c_str());
        bool vip = atoi(parts[3].c_str()) != 0;
        out.push_back(User(id, name, bal, vip));
    }
}

void save_users(const string &path, const vector<User> &v) {
    ostringstream ss;
    ss << "# id|name|balance|is_vip\n";
    for (size_t i = 0; i < v.size(); ++i) ss << v[i].serialize() << "\n";
    write_file(path.c_str(), ss.str());
}

bool product_cmp(Product* a, Product* b) { return a->finalPrice() < b->finalPrice(); }

string make_temp_path(const string &prefix) {
    // prefix should include directory, e.g. /tmp/vip_req_
    char buf[256];
    srand((unsigned int)time(NULL) ^ getpid());
    int r = rand() % 100000;
    snprintf(buf, sizeof(buf), "%s%u_%d.tmp", prefix.c_str(), (unsigned int)time(NULL), r);
    return string(buf);
}
