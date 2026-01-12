// main.cpp
// 入口程序（仅负责调用功能接口，父进程统一控制终端输出）
// 说明：
// - main 仅负责：显示菜单、收集用户输入、写 request 文件（临时）、fork 子进程执行操作、等待子进程结束、读取 result 并打印。
// - 子进程不直接操控终端；所有终端输出由父进程统一打印，避免刷屏和输出交叉。
// - request/result 临时文件由父进程创建并在读取后删除；临时文件名使用 `make_temp_path` 生成。

#include "core.h"
#include "ops.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>

using namespace std;

// 读取 result 文件并返回字符串（由父进程打印）
static string read_result_and_print(const string &result_path) {
    return read_file(result_path.c_str());
}

int main() {
    const string product_path = "data/products.txt"; // 商品数据文件路径
    const string user_path = "data/users.txt";       // 用户数据文件路径

    while (true) {
        // 菜单（父进程打印）
        cout << "\n=== 商品 VIP 购买系统 ===" << endl;
        cout << "1. 列出商品" << endl;
        cout << "2. 添加商品" << endl;
        cout << "3. 列出用户" << endl;
        cout << "4. 注册用户" << endl;
        cout << "5. 购买商品" << endl;
        cout << "6. 升级为 VIP" << endl;
        cout << "0. 退出" << endl;
        cout << "选择: ";
        int choice = -1; if (!(cin >> choice)) break;
        string dummy; getline(cin, dummy); // 消耗行尾

        // request/result 临时路径
        string req_path = "";
        string res_path = make_temp_path("/tmp/vip_res_");

        // 父进程收集需要的输入并写入 request（如果需要）
        if (choice == 2) {
            // 添加商品: type|name|price|extra
            string type, name; double price=0.0, extra=0.0;
            cout << "类型 (P/D): "; if (!(cin >> type)) { cout << "输入错误\n"; continue; } getline(cin, dummy);
            cout << "名称: "; if (!getline(cin, name)) { cout << "输入错误\n"; continue; }
            cout << "价格: "; if (!(cin >> price)) { cout << "价格输入错误\n"; continue; }
            if (type == "P" || type == "p") { cout << "邮费: "; if (!(cin >> extra)) extra = 0.0; }
            ostringstream ss; ss << type << "|" << name << "|" << price << "|" << extra;
            req_path = make_temp_path("/tmp/vip_req_"); write_file(req_path.c_str(), ss.str());
        } else if (choice == 4) {
            // 注册用户: name|balance
            string name; double bal=0.0;
            cout << "姓名: "; if (!getline(cin, name)) { cout << "输入错误\n"; continue; }
            cout << "初始余额: "; if (!(cin >> bal)) { cout << "余额输入错误\n"; continue; }
            ostringstream ss; ss << name << "|" << bal;
            req_path = make_temp_path("/tmp/vip_req_"); write_file(req_path.c_str(), ss.str());
        } else if (choice == 5) {
            // 购买: uid|pid
            int uid, pid; cout << "用户 ID: "; if (!(cin >> uid)) { cout << "输入错误\n"; continue; }
            cout << "商品 ID: "; if (!(cin >> pid)) { cout << "输入错误\n"; continue; }
            ostringstream ss; ss << uid << "|" << pid;
            req_path = make_temp_path("/tmp/vip_req_"); write_file(req_path.c_str(), ss.str());
        } else if (choice == 6) {
            // 升级 VIP: uid
            int uid; cout << "用户 ID: "; if (!(cin >> uid)) { cout << "输入错误\n"; continue; }
            ostringstream ss; ss << uid;
            req_path = make_temp_path("/tmp/vip_req_"); write_file(req_path.c_str(), ss.str());
        }

        // fork 子进程执行操作，子进程只输出到 result 文件
        pid_t pid = fork();
        if (pid < 0) { cerr << "fork 失败" << endl; break; }
        if (pid == 0) {
            if (choice == 1) op_list_products(product_path, string(), res_path);
            else if (choice == 2) op_add_product(product_path, req_path, res_path);
            else if (choice == 3) op_list_users(user_path, string(), res_path);
            else if (choice == 4) op_register_user(user_path, req_path, res_path);
            else if (choice == 5) op_purchase(product_path, user_path, req_path, res_path);
            else if (choice == 6) op_upgrade_vip(user_path, req_path, res_path);
            else if (choice == 0) write_file(res_path.c_str(), string("退出子进程\n"));
            _exit(0);
        } else {
            // 父进程等待并统一打印 result 内容
            int status = 0; waitpid(pid, &status, 0);
            string out = read_result_and_print(res_path);
            if (!out.empty()) cout << out << endl;
            // 清理临时文件
            if (!req_path.empty()) unlink(req_path.c_str());
            if (!res_path.empty()) unlink(res_path.c_str());
        }

        if (choice == 0) break;
    }
    cout << "主程序退出" << endl;
    return 0;

}
