// weak.cc
__attribute__((weak)) double x; // 弱符号
void assign() { x = 3.14159; }  // 以为操作的是weak.cc里的double