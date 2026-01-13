#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstdio>

using namespace std;

/*
 * 工具函数（尽量保持简单）
 * - trim / splitByPipe：文件解析（members.txt / transactions.txt）
 * - dateToInt：把 YYYY-MM-DD -> yyyymmdd（用于存储/比较）
 * - readLineSafe：getline 安全读取
 * - readIntLine / readDoubleLine / readYesNo：交互输入（回车默认）
 *
 */

namespace util {

    inline string trim(const string& s) {
        size_t b = 0;
        while (b < s.size() && (s[b] == ' ' || s[b] == '\t' || s[b] == '\n' || s[b] == '\r')) ++b;
        size_t e = s.size();
        while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t' || s[e - 1] == '\n' || s[e - 1] == '\r')) --e;
        return s.substr(b, e - b);
    }

    inline vector<string> splitByPipe(const string& line) {
        // 文件行格式：字段之间用 | 分隔
        // 字段两侧可能有空格 所以 push 之前必须 trim
        vector<string> parts;
        string buf;
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '|') {
                parts.push_back(trim(buf));
                buf.clear();
            } else {
                buf += line[i];
            }
        }
        parts.push_back(trim(buf));
        return parts;
    }

    // YYYY-MM-DD -> yyyymmdd
    //  year*10000 *1000 好像都有点问题 todo
    inline int dateToInt(const string& date) {
        // date 来自用户输入或文件 可能为空
        if (date.size() != 10) return 0;
        if (date[4] != '-' || date[7] != '-') return 0; 

        int y = atoi(date.substr(0, 4).c_str());
        int m = atoi(date.substr(5, 2).c_str());
        int d = atoi(date.substr(8, 2).c_str());

        // 最小范围校验 没有加闰年
        if (m < 1 || m > 12) return 0;
        if (d < 1 || d > 31) return 0;

        return y * 10000 + m * 100 + d;
    }

    inline string todayDate() {
        // 统一日期输出格式：YYYY-MM-DD
        time_t now = time(nullptr);
        tm* lt = localtime(&now);
        char buf[32];
        sprintf(buf, "%04d-%02d-%02d", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday);
        return string(buf);
    }

    inline void readLineSafe(string& out) {
        // 安全读取一整行 并去掉 Windows 的 '\r'
        getline(cin, out);
        if (!out.empty() && out.back() == '\r') out.pop_back(); 
    }

    inline int readIntLine(const string& prompt, int defaultValue) {
        // 用整行读取 允许用户直接回车用默认值
        while (true) {
            cout << prompt;
            string line;
            readLineSafe(line);
            line = trim(line);
            if (line.empty()) return defaultValue;

            istringstream iss(line);
            int v;
            if (iss >> v) return v;
            cout << "输入无效 请输入整数\n";
        }
    }

    inline double readDoubleLine(const string& prompt, double defaultValue) {
        // 整行读取 + 回车默认
        while (true) {
            cout << prompt;
            string line;
            readLineSafe(line);
            line = trim(line);
            if (line.empty()) return defaultValue;

            istringstream iss(line);
            double v;
            if (iss >> v) return v;
            cout << "输入无效 请输入数字\n";
        }
    }

    inline bool readYesNo(const string& prompt, bool defaultValue) {
        // 允许回车默认
        while (true) {
            cout << prompt;
            string line;
            readLineSafe(line);
            line = trim(line);

            if (line.empty()) return defaultValue;
            if (line == "y" || line == "Y") return true;
            if (line == "n" || line == "N") return false;

            cout << "请输入 y 或 n（或回车默认）\n";
        }
    }

} 
