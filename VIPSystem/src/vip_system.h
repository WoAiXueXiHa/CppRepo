#pragma once
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <cstdlib>

#include "member.h"
#include "transaction.h"
#include "functors.h"
#include "utility.h"

using namespace std;

/*
 * VipSystem：系统核心
 * - mMembers：会员表（memberId -> Member*）
 * - mTransactions：交易记录表（顺序存储）
 * - 文件读写：members.txt / transactions.txt
 *
 * 菜单：
 * 1 新增会员
 * 2 修改会员
 * 3 删除会员（同时删除其交易记录，减少复杂分支）
 * 4 记录消费（多态折扣 + 仿函数积分）
 * 5 查询会员消费明细
 * 0 保存并退出
 * 
 * 设计：
 * 1) 多态：Member* 调用 virtual discountRate()/levelName() 实现不同等级折扣 
 * 2) 仿函数：PointsCalculator 把“积分策略”独立出来，展示可替换策略 
 * 3) 文件持久化：Member/Transaction 各自提供 infoTxt()，VipSystem 负责读写与对象生命周期 
 *
 */

class VipSystem {
private:
    map<string, Member*> mMembers;
    vector<Transaction>  mTransactions;

    long mNextTransactionId = 1;
    functor::PointsCalculator mPointsCalculator;

    string mMemberFilePath;
    string mTransactionFilePath;

private:
    // 防止浅拷贝导致重复释放
    VipSystem(const VipSystem&);
    VipSystem& operator=(const VipSystem&);

public:
    VipSystem(const string& memberFilePath, const string& transactionFilePath)
        : mMemberFilePath(memberFilePath)
        , mTransactionFilePath(transactionFilePath) {}

    ~VipSystem() {
        clearMembers(); // 统一释放 Member*
    }

public:
    void run() {
        loadAll();

        while (true) {
            cout << "\n========== 商场 VIP 消费查询系统 ==========\n";
            cout << "1. 新增会员\n";
            cout << "2. 修改会员\n";
            cout << "3. 删除会员\n";
            cout << "4. 记录消费\n";
            cout << "5. 查询会员消费明细\n";
            cout << "0. 保存并退出\n";
            cout << "------------------------------------------\n";
            cout << "请输入菜单编号：";

            int menu;
            cin >> menu;

            switch (menu) {
                case 1: addMember(); break;
                case 2: editMember(); break;
                case 3: deleteMember(); break;
                case 4: recordPurchase(); break;
                case 5: queryMemberTransactions(); break;
                case 0:
                    saveAll();
                    cout << "已保存，退出 \n";
                    return;
                default:
                    cout << "无效选项 \n";
                    break;
            }

            // cin >> 之后会残留 '\n' 用 ignore 等待回车
            cout << "按回车继续...";
            cin.ignore(1024, '\n');
            string dummy;
            util::readLineSafe(dummy);
        }
    }

private:
    // ================== 内部工具 ==================

    void clearMembers() {
        // 释放 new 出来的 Member*
        for (auto it = mMembers.begin(); it != mMembers.end(); ++it) {
            delete it->second;
            it->second = nullptr;
        }
        mMembers.clear();
    }

    Member* findMember(const string& memberId) const {
        auto it = mMembers.find(memberId);
        if (it == mMembers.end()) return nullptr;
        return it->second;
    }

    bool memberExists(const string& memberId) const {
        return mMembers.find(memberId) != mMembers.end();
    }

    void printMemberSimple(const Member* m) const {
        if (!m) return;
        cout << "会员号=" << m->getId()
             << " 姓名=" << m->getName()
             << " 电话=" << m->getPhone()
             << " 等级=" << m->levelName()
             << " 积分=" << m->getPoints()
             << " 入会=" << m->getJoinDate()
             << "\n";
    }

    void printTransactionSimple(const Transaction& t) const {
        cout << "交易#" << t.transactionId
             << " 日期=" << t.date
             << " 商品=" << t.item
             << " 原价=" << fixed << setprecision(2) << t.amount
             << " 实付=" << fixed << setprecision(2) << t.pay
             << " 积分+" << t.pointsEarned
             << "\n";
    }

    // ================== 文件读写 ==================

    void loadMembers() {
        
        ifstream fin(mMemberFilePath.c_str());
        if (!fin) return; // 第一次运行没有文件很正常

        string line;
        while (getline(fin, line)) {
            line = util::trim(line);
            if (line.empty()) continue;

            // id | name | phone | level | points | joinDate
            vector<string> p = util::splitByPipe(line);
            if (p.size() < 6) continue;

            string id = p[0];
            if (id.empty()) continue;

            string name = p[1];
            string phone = p[2];
            int levelCode = atoi(p[3].c_str());
            int points = atoi(p[4].c_str());
            string joinDate = p[5];

            // 根据 levelCode 创建不同子类对象
            mMembers[id] = createMemberByLevel(levelCode, id, name, phone, points, joinDate);
        }
        fin.close();
    }

    void loadTransactions() {
        mTransactions.clear(); // 避免重复叠加

        ifstream fin(mTransactionFilePath.c_str());
        if (!fin) return;

        string line;
        while (getline(fin, line)) {
            line = util::trim(line);
            if (line.empty()) continue;

            // transactionId | memberId | date | item | amount | pay | pointsEarned
            vector<string> p = util::splitByPipe(line);
            if (p.size() < 7) continue;

            Transaction t;
            t.transactionId = atol(p[0].c_str());
            t.memberId = p[1];
            t.date = p[2];
            t.dateKey = util::dateToInt(t.date); 
            t.item = p[3];
            t.amount = atof(p[4].c_str());
            t.pay = atof(p[5].c_str());
            t.pointsEarned = atoi(p[6].c_str());

            mTransactions.push_back(t);
        }
        fin.close();
    }

    void saveMembers() const {
        ofstream fout(mMemberFilePath.c_str());
        if (!fout) return;

        for (auto it = mMembers.begin(); it != mMembers.end(); ++it) {
            // infoTxt() 的字段顺序必须和 loadMembers() 解析一致
            fout << it->second->infoTxt() << "\n";
        }
    }

    void saveTransactions() const {
        ofstream fout(mTransactionFilePath.c_str());
        if (!fout) return;

        for (size_t i = 0; i < mTransactions.size(); ++i) {
            fout << mTransactions[i].infoTxt() << "\n";
        }
    }

    void loadAll() {
        loadMembers();
        loadTransactions();

        // 自增交易号 避免程序重启后交易号重复
        long maxId = 0;
        for (size_t i = 0; i < mTransactions.size(); ++i) {
            if (mTransactions[i].transactionId > maxId) maxId = mTransactions[i].transactionId;
        }
        mNextTransactionId = maxId + 1;
    }

    void saveAll() const {
        saveMembers();
        saveTransactions();
    }

    // ================== 菜单功能 ==================

    void addMember() {
        cout << "\n[新增会员]\n";
        cout << "会员号：";
        string id;
        cin >> id;

        if (id.empty()) { cout << "会员号不能为空 \n"; return; }
        if (memberExists(id)) { cout << "该会员号已存在 \n"; return; }

        // 下面要 getline 先清掉 cin >> 的换行
        cin.ignore(1024, '\n');

        cout << "姓名：";
        string name; util::readLineSafe(name); name = util::trim(name);

        cout << "电话：";
        string phone; util::readLineSafe(phone); phone = util::trim(phone);

        int levelCode = util::readIntLine("等级(0普通 1VIP 2SVIP 回车默认0)：", 0);
        if (levelCode < 0 || levelCode > 2) levelCode = 0;

        string joinDate = util::todayDate();
        mMembers[id] = createMemberByLevel(levelCode, id, name, phone, 0, joinDate);

        cout << "新增成功 \n";
        printMemberSimple(mMembers[id]);
    }

    void editMember() {
        cout << "\n[修改会员]\n";
        cout << "会员号：";
        string id;
        cin >> id;

        Member* m = findMember(id);
        if (!m) { cout << "未找到该会员 \n"; return; }

        cout << "当前信息：\n";
        printMemberSimple(m);

        cin.ignore(1024, '\n');

        cout << "新姓名(回车不改)：";
        string name; util::readLineSafe(name); name = util::trim(name);
        if (!name.empty()) m->setName(name);

        cout << "新电话(回车不改)：";
        string phone; util::readLineSafe(phone); phone = util::trim(phone);
        if (!phone.empty()) m->setPhone(phone);

        cout << "修改完成：\n";
        printMemberSimple(m);
    }

    void deleteMember() {
        cout << "\n[删除会员]\n";
        cout << "会员号：";
        string id;
        cin >> id;

        auto it = mMembers.find(id);
        if (it == mMembers.end()) { cout << "未找到该会员 \n"; return; }

        cout << "将删除：\n";
        printMemberSimple(it->second);

        if (!util::readYesNo("确认删除？(y/n，回车默认n)：", false)) {
            cout << "已取消 \n";
            return;
        }

        // 删除会员时顺带删除其交易记录 避免孤儿交易->类比孤儿进程
        vector<Transaction> remain;
        remain.reserve(mTransactions.size());
        for (size_t i = 0; i < mTransactions.size(); ++i) {
            if (mTransactions[i].memberId != id) remain.push_back(mTransactions[i]);
        }
        mTransactions.swap(remain);

        // 先 delete 再 erase
        delete it->second;
        mMembers.erase(it);

        cout << "删除成功（含该会员交易记录） \n";
    }

    void recordPurchase() {
        cout << "\n[记录消费]\n";
        cout << "会员号：";
        string id;
        cin >> id;

        Member* m = findMember(id);
        if (!m) { cout << "未找到该会员 \n"; return; }

        cout << "会员等级=" << m->levelName()
             << " 折扣=" << fixed << setprecision(2) << m->discountRate()
             << "\n";

        cin.ignore(1024, '\n');

        cout << "消费日期(YYYY-MM-DD 回车默认今天)：";
        string date; util::readLineSafe(date); date = util::trim(date);
        if (date.empty()) date = util::todayDate();

        // 日期不合法时 直接回退到今天
        if (util::dateToInt(date) == 0) {
            cout << "日期格式不合法，已自动改为今天 \n";
            date = util::todayDate();
        }

        cout << "商品/备注(回车默认未填写)：";
        string item; util::readLineSafe(item); item = util::trim(item);
        if (item.empty()) item = "未填写";

        double amount = util::readDoubleLine("原价金额：", 0.0);
        if (amount < 0) amount = 0.0;

        // 不同等级折扣不同
        double pay = amount * m->discountRate();

        // 仿函数积分策略
        int points = mPointsCalculator(pay);

        Transaction t;
        t.transactionId = mNextTransactionId++;
        t.memberId = id;
        t.date = date;
        t.dateKey = util::dateToInt(date);
        t.item = item;
        t.amount = amount;
        t.pay = pay;
        t.pointsEarned = points;

        mTransactions.push_back(t);
        m->addPoints(points);

        cout << "记录成功：实付=" << fixed << setprecision(2) << pay
             << " 积分+" << points
             << " 当前积分=" << m->getPoints()
             << "\n";
    }

    void queryMemberTransactions() {
        cout << "\n[查询会员消费明细]\n";
        cout << "会员号：";
        string id;
        cin >> id;

        Member* m = findMember(id);
        if (!m) { cout << "未找到该会员 \n"; return; }

        cout << "会员信息：\n";
        printMemberSimple(m);

        // 收集该会员交易
        vector<Transaction> list;
        for (size_t i = 0; i < mTransactions.size(); ++i) {
            if (mTransactions[i].memberId == id) list.push_back(mTransactions[i]);
        }

        if (list.empty()) {
            cout << "暂无消费记录 \n";
            return;
        }

        cout << "消费记录：\n";
        double sumPay = 0.0;
        int sumPoints = 0;

        for (size_t i = 0; i < list.size(); ++i) {
            printTransactionSimple(list[i]);
            sumPay += list[i].pay;
            sumPoints += list[i].pointsEarned;
        }

        cout << "合计：实付=" << fixed << setprecision(2) << sumPay
             << " 本次累计积分=" << sumPoints
             << "\n";
    }
};
