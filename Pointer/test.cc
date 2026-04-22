#include <iostream>

struct A {
    char a;
    int b;
    double c;
};

int main() {
    std::cout << sizeof(A) << std::endl;
    return 0;
}