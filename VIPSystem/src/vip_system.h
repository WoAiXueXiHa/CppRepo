#pragma once

#include <fstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <cstdlib>

#include "member.h"
#include "transaction.h"
#include "functors.h"
#include "utility.h"

using namespace std;
/*
 * VipSystem 系统核心类
 * - mMembers: 会员表（会员号 -> Member*）
 * - mTransactions: 交易记录表
 * - 文件读写：members.txt / transactions.txt
 */

class VipSystem {
private:
    map<string, Member*> mMembers;        // 哈希映射 KV模型
    vector<Transaction>       mTransactions;

    long mNextTransactionId;
    functor::PointsCalculator mPointsCalculator;

    string mMemberFilePath;
    string mTransactionFilePath;

private:
    // 禁拷贝：避免浅拷贝导致重复释放
    VipSystem(const VipSystem &);
    VipSystem& operator=(const VipSystem &);

public:
    VipSystem(const string &memberFilePath, const string &transactionFilePath)
        :mNextTransactionId(1)
        ,mMemberFilePath(memberFilePath)
        ,mTransactionFilePath(transactionFilePath)
        {}
    void clearMembers() {
        auto it = mMembers.begin();
        while(it != mMembers.end()){
            delete it->second;
            it->second = NULL;
            it++;
        }
        mMembers.clear();
    }

    ~VipSystem(){ clearMembers(); }

public:
    void run();

private:
    // 文件读写
    void loadMembers(){
        ifstream fin(mMemberFilePath.c_str());
        if (!fin) return; // 首次运行没有文件是正常情况

        string line;
        while (getline(fin, line)) {
            line = util::trim(line);
            if (line.empty()) continue;

            // id | name | phone | level | points | joinDate
            vector<string> parts = util::splitByPipe(line);
            if (parts.size() < 6) continue;

            string id = parts[0];
            string name = parts[1];
            string phone = parts[2];
            int levelCode = atoi(parts[3].c_str());
            int points = atoi(parts[4].c_str());
            string joinDate = parts[5];

            if (id.empty()) continue;

            // 覆盖同 ID
            map<string, Member*>::iterator it = mMembers.find(id);
            if (it != mMembers.end()) {
                delete it->second;
                mMembers.erase(it);
            }

            mMembers[id] = createMemberByLevel(levelCode, id, name, phone, points, joinDate);
        }
        fin.close();    
    }

    void loadTransactions(){
        ifstream fin(mTransactionFilePath.c_str());
        if (!fin) return;

        string line;
        while (getline(fin, line)) {
            line = util::trim(line);
            if (line.empty()) continue;

            // transactionId|memberId|date|item|amount|pay|pointsEarned
            vector<string> parts = util::splitByPipe(line);
            if (parts.size() < 7) continue;

            Transaction t;
            t.transactionId = atol(parts[0].c_str());
            t.memberId = parts[1];
            t.date = parts[2];
            t.dateKey = util::dateToInt(t.date);
            t.item = parts[3];
            t.amount = atof(parts[4].c_str());
            t.pay = atof(parts[5].c_str());
            t.pointsEarned = atoi(parts[6].c_str());

            mTransactions.push_back(t);
        }
        fin.close();
    }

    void saveMembers() const{
        ofstream fout(mMemberFilePath.c_str());
        if (!fout) return;

        for (map<string, Member*>::const_iterator it = mMembers.begin(); it != mMembers.end(); ++it) {
            fout << it->second->infoTxt() << "\n";
        }
        fout.close();
    }

    void saveTransactions() const{
        ofstream fout(mTransactionFilePath.c_str());
        if (!fout) return;

        for (size_t i = 0; i < mTransactions.size(); ++i) {
            fout << mTransactions[i].infoTxt() << "\n";
        }
        fout.close();
    }

    void loadAll(){
        loadMembers();
        loadTransactions();

        // 恢复 nextTransactionId（保证自增不重复）
        long maxId = 0;
        for (size_t i = 0; i < mTransactions.size(); ++i) {
            if (mTransactions[i].transactionId > maxId) {
                maxId = mTransactions[i].transactionId;
            }
        }
        mNextTransactionId = maxId + 1;
    }
    void saveAll() const{
        saveMembers();
        saveTransactions();
    }

    // 内部工具
    void clearMembers();
    Member* findMember(const string &memberId) const{
        auto it = mMembers.find(memberId);
        if(it == mMembers.end()) return nullptr;
        return it->second;
    }
    bool memberExists(const string &memberId) const{
        return mMembers.find(memberId) != mMembers.end();
    }

    // 终端提示
    void printTitle(const string &title) const{
        cout << "\n====================================================\n";
        cout << "  " << title << "\n";
        cout << "====================================================\n";
    }
    void printMemberTableHeader() const{
        cout << left << setw(12) << "会员号"
                  << left << setw(10) << "姓名"
                  << left << setw(14) << "电话"
                  << left << setw(8)  << "等级"
                  << right << setw(8) << "积分"
                  << "   " << "入会日期"
                  << "\n----------------------------------------------------\n";
    }

    void printTransactionTableHeader() const{
        cout << left << setw(10) << "交易号"
             << left << setw(12) << "会员号"
             << left << setw(12) << "日期"
             << left << setw(16) << "商品/备注"
             << right << setw(10) << "原价"
             << right << setw(10) << "实付"
             << right << setw(8)  << "积分"
             << "\n----------------------------------------------------\n";
    }

    // 业务功能
    void addMember(){
        printTitle("新增会员");

        cout << "请输入会员号（例如 M1001）：";
        string memberId;
        cin >> memberId;

        if (memberId.empty()) {
            cout << "会员号不能为空。\n";
            return;
        }
        if (memberExists(memberId)) {
            cout << "该会员号已存在。\n";
            return;
        }

        cin.ignore(1024, '\n');

        cout << "请输入姓名：";
        string name;
        util::readLineSafe(name);
        name = util::trim(name);

        cout << "请输入电话：";
        string phone;
        util::readLineSafe(phone);
        phone = util::trim(phone);

        int levelCode = util::readIntLine("请选择等级（0=普通 1=VIP 2=SVIP，回车默认0）：", 0);
        if (levelCode < 0 || levelCode > 2) levelCode = 0;

        string joinDate = util::todayDate();
        int points = 0;

        mMembers[memberId] = createMemberByLevel(levelCode, memberId, name, phone, points, joinDate);
        cout << "新增成功，入会日期：" << joinDate << "\n";
    }

    void editMember() {
        printTitle("修改会员");

        cout << "请输入会员号：";
        string memberId;
        cin >> memberId;

        Member *member = findMember(memberId);
        if (!member) {
            cout << "未找到该会员。\n";
            return;
        }

        cout << "当前信息：\n";
        printMemberTableHeader();
        member->printRow();

        cin.ignore(1024, '\n');

        cout << "请输入新姓名（回车不改）：";
        string name;
        util::readLineSafe(name);
        name = util::trim(name);
        if (!name.empty()) member->setName(name);

        cout << "请输入新电话（回车不改）：";
        string phone;
        util::readLineSafe(phone);
        phone = util::trim(phone);
        if (!phone.empty()) member->setPhone(phone);

        cout << "修改完成。\n";
    }

    void deleteMember() {
        printTitle("删除会员");

        cout << "请输入会员号：";
        string memberId;
        cin >> memberId;

        map<string, Member*>::iterator it = mMembers.find(memberId);
        if (it == mMembers.end()) {
            cout << "未找到该会员。\n";
            return;
        }

        bool confirm = util::readYesNo("确认删除该会员？(y/n，回车默认n)：", false);
        if (!confirm) {
            cout << "已取消。\n";
            return;
        }

        bool deleteTransactions = util::readYesNo("是否同时删除该会员全部消费记录？(y/n，回车默认n)：", false);
        if (deleteTransactions) {
            vector<Transaction> remaining;
            remaining.reserve(mTransactions.size());
            for (size_t i = 0; i < mTransactions.size(); ++i) {
                if (mTransactions[i].memberId != memberId) {
                    remaining.push_back(mTransactions[i]);
                }
            }
            mTransactions.swap(remaining);
        }

        delete it->second;
        mMembers.erase(it);

        cout << "删除成功。\n";
    }

    void listMembers() const {
        printTitle("会员列表");

        if (mMembers.empty()) {
            cout << "暂无会员。\n";
            return;
        }

        printMemberTableHeader();
        for (map<string, Member*>::const_iterator it = mMembers.begin(); it != mMembers.end(); ++it) {
            it->second->printRow();
        }
    }

/**************** 业务：交易与查询 ****************/
    void recordPurchase() {
        printTitle("记录消费");

        cout << "请输入会员号：";
        string memberId;
        cin >> memberId;

        Member *member = findMember(memberId);
        if (!member) {
            cout << "未找到该会员。\n";
            return;
        }

        cout << "会员等级：" << member->levelName()
            << "，折扣系数：" << fixed << setprecision(2) << member->discountRate() << "\n";

        cin.ignore(1024, '\n');

        cout << "请输入消费日期（YYYY-MM-DD，回车默认今天）：";
        string date;
        util::readLineSafe(date);
        date = util::trim(date);
        if (date.empty()) date = util::todayDate();

        cout << "请输入商品/备注（回车默认“未填写”）：";
        string item;
        util::readLineSafe(item);
        item = util::trim(item);
        if (item.empty()) item = "未填写";

        double amount = util::readDoubleLine("请输入原价金额：", 0.0);
        if (amount < 0) amount = 0.0;

        // 动态多态：基类指针调用 virtual 折扣函数
        double pay = amount * member->discountRate();

        // 仿函数：积分策略
        int pointsEarned = mPointsCalculator(pay);

        Transaction t;
        t.transactionId = mNextTransactionId++;
        t.memberId = memberId;
        t.date = date;
        t.dateKey = util::dateToInt(date);
        t.item = item;
        t.amount = amount;
        t.pay = pay;
        t.pointsEarned = pointsEarned;

        mTransactions.push_back(t);
        member->addPoints(pointsEarned);

        cout << "记录成功：实付 " << fixed << setprecision(2) << pay
            << "，积分 +" << pointsEarned
            << "，当前总积分 " << member->getPoints() << "\n";
    }

    void queryMemberTransactions() {
        printTitle("查询会员消费明细");

        cout << "请输入会员号：";
        string memberId;
        cin >> memberId;

        Member *member = findMember(memberId);
        if (!member) {
            cout << "未找到该会员。\n";
            return;
        }

        cout << "会员信息：\n";
        printMemberTableHeader();
        member->printRow();

        vector<Transaction> memberTransactions;
        for (size_t i = 0; i < mTransactions.size(); ++i) {
            if (mTransactions[i].memberId == memberId) {
                memberTransactions.push_back(mTransactions[i]);
            }
        }

        if (memberTransactions.empty()) {
            cout << "该会员暂无消费记录。\n";
            return;
        }

        bool sortByPay = util::readYesNo("是否按实付金额降序排序？(y/n，回车默认n)：", false);
        if (sortByPay) {
            sort(memberTransactions.begin(), memberTransactions.end(), functor::PayDescending());
        }

        cout << "\n消费明细：\n";
        printTransactionTableHeader();

        double totalAmount = 0.0;
        double totalPay = 0.0;
        int totalPoints = 0;

        for (size_t i = 0; i < memberTransactions.size(); ++i) {
            const Transaction &t = memberTransactions[i];

            cout << left << setw(10) << t.transactionId
                << left << setw(12) << t.memberId
                << left << setw(12) << t.date
                << left << setw(16) << t.item.substr(0, 15)
                << right << setw(10) << fixed << setprecision(2) << t.amount
                << right << setw(10) << fixed << setprecision(2) << t.pay
                << right << setw(8)  << t.pointsEarned
                << "\n";

            totalAmount += t.amount;
            totalPay += t.pay;
            totalPoints += t.pointsEarned;
        }

        cout << "----------------------------------------------------\n";
        cout << "合计：原价=" << fixed << setprecision(2) << totalAmount
            << "，实付=" << fixed << setprecision(2) << totalPay
            << "，累计积分=" << totalPoints << "\n";
    }

    void VipSystem::queryTransactionsByDateRange() {
        printTitle("按日期范围查询消费记录");

        cin.ignore(1024, '\n');

        cout << "请输入起始日期（YYYY-MM-DD）：";
        string beginDate;
        util::readLineSafe(beginDate);
        beginDate = util::trim(beginDate);

        cout << "请输入结束日期（YYYY-MM-DD）：";
        string endDate;
        util::readLineSafe(endDate);
        endDate = util::trim(endDate);

        int beginKey = util::dateToInt(beginDate);
        int endKey = util::dateToInt(endDate);

        if (beginKey == 0 || endKey == 0 || beginKey > endKey) {
            cout << "日期输入不合法或范围错误。\n";
            return;
        }

        functor::DateRange predicate(beginKey, endKey);

        vector<Transaction> results;
        for (size_t i = 0; i < mTransactions.size(); ++i) {
            if (predicate(mTransactions[i])) {
                results.push_back(mTransactions[i]);
            }
        }

        if (results.empty()) {
            cout << "该日期范围内无记录。\n";
            return;
        }

        bool sortByPay = util::readYesNo("是否按实付金额降序排序？(y/n，回车默认n)：", false);
        if (sortByPay) {
            sort(results.begin(), results.end(), functor::PayDescending());
        }

        cout << "\n查询结果：\n";
        printTransactionTableHeader();

        double totalPay = 0.0;
        for (size_t i = 0; i < results.size(); ++i) {
            const Transaction &t = results[i];

            cout << left << setw(10) << t.transactionId
                << left << setw(12) << t.memberId
                << left << setw(12) << t.date
                << left << setw(16) << t.item.substr(0, 15)
                << right << setw(10) << fixed << setprecision(2) << t.amount
                << right << setw(10) << fixed << setprecision(2) << t.pay
                << right << setw(8)  << t.pointsEarned
                << "\n";

            totalPay += t.pay;
        }

        cout << "----------------------------------------------------\n";
        cout << "该范围内实付合计：" << fixed << setprecision(2) << totalPay << "\n";
    }

    void showTopNSpenders() {
        printTitle("消费排行榜 TopN（按实付汇总）");

        cin.ignore(1024, '\n');
        int topN = util::readIntLine("请输入 N（回车默认 5）：", 5);
        if (topN <= 0) topN = 5;

        // memberId -> totalPay
        map<string, double> totalPayByMember;
        for (size_t i = 0; i < mTransactions.size(); ++i) {
            totalPayByMember[mTransactions[i].memberId] += mTransactions[i].pay;
        }

        struct SpendingRank {
            string memberId;
            double pay;
            SpendingRank() : pay(0.0) {}
        };

        vector<SpendingRank> ranks;
        for (map<string, double>::iterator it = totalPayByMember.begin(); it != totalPayByMember.end(); ++it) {
            SpendingRank r;
            r.memberId = it->first;
            r.pay = it->second;
            ranks.push_back(r);
        }

        if (ranks.empty()) {
            cout << "暂无交易记录。\n";
            return;
        }

        sort(ranks.begin(), ranks.end(), functor::PayDescending());

        cout << left << setw(6)  << "排名"
            << left << setw(12) << "会员号"
            << left << setw(10) << "姓名"
            << left << setw(8)  << "等级"
            << right << setw(12) << "实付总额"
            << "\n----------------------------------------------------\n";

        int limit = (topN < (int)ranks.size()) ? topN : (int)ranks.size();
        for (int i = 0; i < limit; ++i) {
            Member *member = findMember(ranks[i].memberId);

            string name = member ? member->getName() : "未知";
            string level = member ? member->levelName() : "-";

            cout << left << setw(6)  << (i + 1)
                << left << setw(12) << ranks[i].memberId
                << left << setw(10) << name.substr(0, 9)
                << left << setw(8)  << level
                << right << setw(12) << fixed << setprecision(2) << ranks[i].pay
                << "\n";
        }
    }

/**************** 主循环 ****************/
    void run() {
        loadAll();

        while (true) {
            printTitle("商场 VIP 消费查询系统（命名规范多文件版）");
            cout << "1. 新增会员\n";
            cout << "2. 修改会员\n";
            cout << "3. 删除会员\n";
            cout << "4. 记录消费\n";
            cout << "5. 查询会员消费明细\n";
            cout << "6. 按日期范围查询消费记录\n";
            cout << "7. 消费排行榜 TopN\n";
            cout << "8. 查看会员列表\n";
            cout << "0. 保存并退出\n";
            cout << "----------------------------------------------------\n";
            cout << "请输入菜单编号：";

            int menu;
            cin >> menu;

            switch (menu) {
                case 1: addMember(); break;
                case 2: editMember(); break;
                case 3: deleteMember(); break;
                case 4: recordPurchase(); break;
                case 5: queryMemberTransactions(); break;
                case 6: queryTransactionsByDateRange(); break;
                case 7: showTopNSpenders(); break;
                case 8: listMembers(); break;
                case 0:
                    saveAll();
                    cout << "数据已保存，程序退出。\n";
                    return;
                default:
                    cout << "无效选项。\n";
                    break;
            }

            cout << "\n按回车继续...";
            cin.ignore(1024, '\n');
            string dummy;
            util::readLineSafe(dummy);
        }
    }

};

