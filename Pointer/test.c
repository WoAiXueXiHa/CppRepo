//#include <iostream>

// int main() {
//     int arr[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
//     std::cout << "&arr: " << &arr << std::endl;
//     std::cout << "&arr+1: " << &arr+1 << std::endl;
//     std::cout << "arr: " << arr << std::endl;
//     std::cout << "arr+1: " << arr + 1 << std::endl;
//     std::cout << "*(arr+1): " << *(arr+1) << std::endl;
//     return 0;
// }

#include <stdio.h>
int main() {
    int a[5] = { 1, 2, 3, 4, 5 };
    int *ptr = (int *)(&a + 1);
    printf( "%d,%d", *(a + 1), *(ptr - 1));
    return 0;
}