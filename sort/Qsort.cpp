#include <iostream>
#include <vector>

using namespace std;

void Qsort1(vector<int>& arr, int left, int right) {
    if(left >= right) return;   // 数组只有一个或没有元素，无需排序
    int i = left, j = right;
    int key = arr[left + (right - left) / 2];
    // 对撞双指针
    // i向右找大于key的元素，找到停下
    // j向左找小于key的元素，找到停下
    // 交换二者元素--->一趟下来，以key为分界线，把整个数组分成两个子数组
    while(i <= j) {
        while(arr[i] < key)  i++;
        while(arr[j] > key)  j--; 
        if(i <= j) swap(arr[i++], arr[j--]);
    }
    // left     j   i    right
    // 递归不断划分子数组
    if(left < j) Qsort1(arr, left, j );
    if(i < right) Qsort1(arr, i,right);
}

void Qsort2(vector<int>& arr, int left, int right) {
    if(left >= right) return;
    // 用三个指针，把数组划分为三个区间
    // left     l       r       right
    // [left,l-1]   严格小于key
    // [l,r-1]      严格等于key
    // [r,right]    严格大于key
    int i = left, l = left - 1, r = right + 1;
    int key = arr[left + (right - left) / 2];
    while(i < r) {
        // l要向左扩张，把小于key的元素全部抱进来
        if(arr[i] < key) swap(arr[++l], arr[i++]);
        else if(arr[i] == key) i++;
        else swap(arr[--r], arr[i]);    // 这里不能i++，把i右边的元素交换过来，这个元素没有处理呢
    }
    // left     l       r       right
    // 递归下去
    Qsort2(arr, left, l);
    Qsort2(arr, r, right);
}

int main() {
    vector<int> arr = {9,5,4,6,7,9,24,6,12,4,6,7};
    //Qsort1(arr, 0, arr.size() - 1);
    Qsort2(arr, 0, arr.size() - 1);
    for(int x : arr) cout << x << " ";
    return 0;
}