#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;
/*
 * 通用工具函数
 * - 字符串 trim / split
 * - 日期处理（YYYY-MM-DD -> yyyymmdd）
 * - 安全行读取（getline）
 * - 基础输入读取（int/double，允许回车默认值）
 */

namespace util {
    // 提取有效字符串
    string trim(const string &s){
        size_t begin = 0;
        while(begin < s.size() 
                && (s[begin] == ' ' || s[begin] == '\t' ||
                    s[begin] == '\n' || s[begin] == '\r')){
            begin++;
        }

        size_t end = s.size();
        while(end > begin 
                && (s[end - 1] == ' ' || s[end - 1] == '\t' ||
                    s[end - 1] == '\n' || s[end - 1] == '\r')){
            end--;
        }
        return s.substr(begin, end - begin);
    }

    vector<string> splitByPipe(const string &line){
        vector<string> parts;
        string buffer;

        for(int i = 0; i < line.size(); i++){
            if(line[i] == '|'){
                parts.push_back(trim(buffer));
                buffer.clear();
            }else{
                buffer += line[i];
            }
        }
        parts.push_back(trim(buffer));
        return parts;
    }

    // YYYY-MM-DD
    int dateToInt(const string &date){
        if(date.size() != 10) return 0;
        int year = atoi(date.substr(0,4).c_str());
        int month = atoi(date.substr(5,2).c_str());
        int day = atoi(date.substr(8,2).c_str());
        
        return year * 1000 + month * 100 + day;
    
    }

    string todayDate(){
        time_t now = time(NULL);
        tm* lt = localtime(&now);
        char buffer[64];
        sprintf(buffer,"%04d-%02d-%02d",
                lt->tm_year + 1900,
                lt->tm_mon + 1,
                lt->tm_mday);
        
        return string(buffer);
    }

    void readLineSafe(string &out){
        getline(cin,out);
        if(!out.empty() && out[out.size() - 1] == '\r')
            out = out.substr(0,out.size() - 1);
    }

    // 把任意可输出类型转成 string
    template <typename T>
    string toString(const T &value) {
        ostringstream oss;
        oss << value;
        return oss.str();
    }

    // 从一整行读取整数 空行返回 defaultValue 非法输入会反复提示
    int readIntLine(const string &prompt, int defaultValue){
        while(1){
            cout << prompt;
            string line;
            readLineSafe(line);
            line = trim(line);

            if(line.empty()) return defaultValue;

            istringstream iss(line);
            int value = 0;
            if(iss >> value) return value;

            cout << "输入无效，请输入整数！\n";
        }
    }

    // 从一整行读取 double 空行返回 defaultValue 非法输入会反复提示
    double readDoubleLine(const string &prompt, double defaultValue){
         while (1) {
            cout << prompt;
            string line;
            readLineSafe(line);
            line = trim(line);

            if (line.empty()) return defaultValue;

            istringstream iss(line);
            double value;
            if (iss >> value) return value;

            cout << "输入无效，请输入数字！\n";
        }
    }

    // 读取 y/n 返回 true/false
    bool readYesNo(const string &prompt, bool defaultValue){
             while (1) {
            cout << prompt;
            string line;
            readLineSafe(line);
            line = trim(line);

            if (line.empty()) return defaultValue;
            if (line == "y" || line == "Y") return true;
            if (line == "n" || line == "N") return false;

            cout << "请输入 y 或 n（或直接回车使用默认值）\n";
        }
    }

};

