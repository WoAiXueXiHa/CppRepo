#include <iostream>
#include <vector>

using namespace std;
vector<int> tmp;
void mergeSort(vector<int>& arr, int left, int right) {
    if(left >= right) return;
    // 1. 划分区间
    // left     mid     right
    int mid = left + (right - left) / 2;
    // 2. 左右区间排序
    mergeSort(arr,left, mid);
    mergeSort(arr, mid + 1, right);
    // 3. 合并两个有序数组，把数组定义成全局的，不用每次递归创建新的数组
    int cur1 = left, cur2 = mid + 1;
    int i = left;
    while(cur1 <= mid && cur2 <= right) {
        tmp[i++] = arr[cur1] <= arr[cur2] ? arr[cur1++] : arr[cur2++];
    }
    // 总会有个数组没走完
    while(cur1 <= mid) tmp[i++] = arr[cur1++];
    while(cur2 <= right) tmp[i++] = arr[cur2++];
    // 还原整个数组
    for(int i = left; i <= right; i++) {
        arr[i] = tmp[i];
    }
}

int main() {
    vector<int> arr = {9,5,4,6,7,9,24,6,12,4,6,7};
    tmp.resize(arr.size());
    mergeSort(arr, 0, arr.size() - 1);
    for(int x : arr) cout << x << " ";
    return 0;
}