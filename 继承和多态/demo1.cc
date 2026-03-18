#include <iostream>
class Hero {
protected:      // 允许子类访问，但不允许外部访问
    std::string _name;
    int _hp;
public:
    Hero(std::string n, int h) :_name(n), _hp(h) {
    	std::cout << "Hero基础构造完成: " << _name << std::endl;
    }

    // 普通方法，没有virtual
    void attack() { std::cout << "Hero 平a！" << std::endl; }
    // 虚方法，允许子类重写
    virtual void skill() { std::cout << "Hero 释放一技能！" << std::endl; }
};

// 派生出法师类，多了个魔法值属性
// 构造要接受name、hp、mp参数，怎么写构造？
class Mage : public Hero {
private:
    int _mp;            // 名字和生命已经继承父类了，魔法值新增自己独有
public:
    // 继承了父类存在属性的构造，直接调用父类构造
    // 自己新增的属性，直接初始化即可
    Mage(std::string n, int h, int m) : Hero(n, h), _mp(m) {
        std::cout << "法师构造完成: " << _name << std::endl;
    } 

    void setMp(int m) { _mp = m; }

    // 1. 隐藏/重定义，名字相同，遮蔽了父类的attack
    void attack() { std::cout << "Mage 平a！" << std::endl; }
    // 2. 覆盖/重写，重写父类的虚函数
    // override 让编译器检查函数前面有没有写错
    void skill() override{ std::cout << "Mage 释放一技能！" << std::endl; }

    void combo() {
        std::cout << "---- 连招开始 ----" << std::endl;
        skill();           // 调用自己的
        Hero::attack();    // 调用父类被隐藏的方法
    }
};

void useSkill(Hero& h) { h.skill(); }

void restoreMp(Hero* hptr) {
    Mage* mptr = dynamic_cast<Mage*>(hptr);
    if(mptr != nullptr) {
        // 转换成功，说明hptr指向的就是法师
        std::cout << "是法师，准备回蓝！" << std::endl;
        mptr->setMp(100);
    } else {
        std::cout << "不是法师，无法回蓝！" << std::endl;
    }
}

int main(){
    // Mage myMage("Medivh", 100, 200);
    // Hero myHero = myMage; // 按值赋值
    // // // 父类对象赋值给子类对象会发生什么？ 禁止！
    // // Hero h1 = Hero("张三", 100);
    // // Mage m1 = Mage("李四", 100, 200);
    // // m1 = h1;
    // myHero.attack();      
    // myMage.skill();       
    // myMage.combo();   
    
    // 正确方式：传指针或引用
    Mage myMage("Medivh", 100, 200 );
    useSkill(myMage);
    return 0;
}