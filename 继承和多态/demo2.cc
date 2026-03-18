#include <iostream>
#include <vector>

// class Hero {
// public: 
//     // 1. 父类开放虚函数
//     virtual void skill() { std::cout << "Hero skill" << std::endl; }
// };

// class Mage : public Hero {  // 2. 存在继承
// public: 
//     // 3. 子类重写
//     void skill() override { std::cout << "Mage skill" << std::endl; }
// };

// void play(Hero& h) {    // 传值会发生切片！
//     h.skill();  // 4. 父类指针/引用调用
// }

// int main() {
//     Mage myMage;
//     play(myMage);
//     return 0;
// }


// 没有多态
// 糟糕的设计
// enum RoleType { WARRIOR, MAGE };

// class Role {
// public:
//     RoleType type;
//     // 没有虚函数！
// };

// // 核心结算引擎代码
// void releaseAllSkills(std::vector<Role*> roles) {
//     for (int i = 0; i < roles.size(); i++) {
//         // 每次都要判断类型！
//         if (roles[i]->type == WARRIOR) {
//             // 强转成战士，调战士技能...
//         } else if (roles[i]->type == MAGE) {
//             // 强转成法师，调法师技能...
//         }
//     }
// }


// // 有多态
// // 优雅的设计
// class Role {
// public:
//     virtual void skill() = 0; // USB 接口定义好了！
// };

// class Warrior : public Role {
// public:
//     void skill() override { /* 战士砍人 */ }
// };

// class Mage : public Role {
// public:
//     void skill() override { /* 法师放火 */ }
// };

// // 核心结算引擎代码（永远不需要再修改了！）
// void releaseAllSkills(std::vector<Role*> roles) {
//     for (int i = 0; i < roles.size(); i++) {
//         roles[i]->skill(); // 直接调！多态会通过 vptr 自动查表找到真实的技能！
//     }
// }


// 父类析构必须设置为虚函数
class Hero {
public:
    Hero() { std::cout << "Hero()" << std::endl; }
    ~Hero() { std::cout << "~Hero()" << std::endl; }
};

class Mage : public Hero {
private:
    int* magicWand;
public:
    Mage() { 
        magicWand = new int[100];
        std::cout << "Mage()" << std::endl; 
    }
    ~Mage() { 
        delete[] magicWand;
        std::cout << "~Mage()" << std::endl; 
    }
};

int main() {
    Hero* h = new Mage();
    std::cout << "-------游戏结束，释放资源-------" << std::endl;
    delete h;
    return 0;
}