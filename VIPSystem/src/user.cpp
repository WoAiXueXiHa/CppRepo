#include "../include/user.h"
#include <sstream>
#include <cstdlib>

User::User() : Person(), _vipLevel(0), _points(0) {}
User::User(int id, const std::string &name, int vipLevel, int points)
    : Person(id, name), _vipLevel(vipLevel), _points(points) {}

int User::getVipLevel() const { return _vipLevel; }
int User::getPoints() const { return _points; }
void User::addPoints(int p) { _points += p; }

std::string User::toCsv() const {
    std::ostringstream oss;
    // 使用访问器以保持封装
    oss << getId() << "," << getName() << "," << _vipLevel << "," << _points;
    return oss.str();
}

User User::fromCsv(const std::string &line) {
    std::istringstream iss(line);
    std::string token;
    int id = 0; std::string name = ""; int vip = 0; int pts = 0;
    if (std::getline(iss, token, ',')) id = atoi(token.c_str());
    if (std::getline(iss, token, ',')) name = token;
    if (std::getline(iss, token, ',')) vip = atoi(token.c_str());
    if (std::getline(iss, token, ',')) pts = atoi(token.c_str());
    return User(id, name, vip, pts);
}
