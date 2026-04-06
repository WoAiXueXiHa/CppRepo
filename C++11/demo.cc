#include <iostream>
using namespace std;

class IntHolder {
private:
    int* ptr;
public:
    IntHolder(int value) : ptr(new int(value)) {
        cout << "构造申请资源\n";
    }
    ~IntHolder() {
        cout << "析构释放资源\n";
        delete ptr;
    }
};
void func() {
    IntHolder obj(100);
    throw runtime_error("出错");
}
int main() {
    try {
        func();
    } catch (const exception& e) {
        cout << "捕获到异常: " << e.what() << endl;
    }
    return 0;
}