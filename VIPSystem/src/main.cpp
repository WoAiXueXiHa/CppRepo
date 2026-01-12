#include "vip_system.h"

int main() {
    VipSystem system("members.txt", "transactions.txt");
    system.run();
    return 0;
}
