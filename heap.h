#ifndef HEAP_H
#define HEAP_H

#include <functional>
#include "vector.h"

#define TTC template <typename T, typename _Cp>
#define hp Heap<T, _Cp>
#define vec Vector<T>

//#define _data vec::_data
//#define _size vec::_size
//#define _capacity vec::_capacity

#define MinHeap(T) Heap<T, std::greater<T>>
#define MaxHeap(T) Heap<T, std::less<T>>

template <typename T, typename _Cp = std::greater<T>>
class Heap : public Vector<T> {
  using vec::_capacity;
  using vec::_data;
  using vec::_size;

 protected:
  // T* _data;
  // int _size, _capacity;
  _Cp _cp;  // 比较函数

  void _init(int cp = DEFAULT_SIZE, int sz = 0) { vec::_init(cp, sz); }
  void siftDown(int start, int m);  // 调整算法
  void siftUp(int start);           // 调整算法

 public:
  Heap() : vec() {}
  Heap(T* arr, int n);

  const T& top() { return _data[0]; }  // 获取顶部元素
  T pop();                             // 删除最小元素
  void push(const T& x);
};

TTC hp::Heap(T* arr, int n) : Vector<T>(arr, n) {
  // Copy from Array
  for (int i = (_size - 2) / 2; i >= 0; i--)  // 逐步向上扩大堆
    siftDown(i, _size - 1);                   // 局部自上向下下滑调整
}

TTC void hp::siftDown(int start, int m) {
  int i = start, j = 2 * i + 1;  // j是i的左子女位置
  T temp = _data[i];
  while (j <= m) {  // 检查是否到最后位置
    if (j < m && _cp(_data[j], _data[j + 1]))
      j++;  // 让j指向两子女中的小者
    if (!_cp(temp, _data[j]))
      break;  // 小则不做调整
    else {    // 否则小者上移, i, j下降
      _data[i] = _data[j];
      i = j, j = 2 * j + 1;
    }
  }
  _data[i] = temp;  // 回放temp中暂存的元素
}

TTC void hp::siftUp(int start) {
  int j = start, i = (j - 1) / 2;
  T temp = _data[j];
  while (j > 0) {  // 沿父结点路径向上直达根
    if (!_cp(_data[i], temp))
      break;  // 父结点值小, 不调整
    else {    // 父结点结点值大, 调整
      _data[j] = _data[i];
      j = i, i = (i - 1) / 2;
    }
  }
  _data[j] = temp;  // 回送
}

TTC T hp::pop() {
  T ret = _data[0];
  _data[0] = _data[--_size];
  siftDown(0, _size);  // 自上向下调整为堆
  return ret;          // 返回最小元素
}

TTC void hp::push(const T& x) {
  vec::push_back(x);
  siftUp(_size - 1);  // 向上调整
}

#undef TTC
#undef hp
#undef vec

#undef _data
#undef _size
#undef _capacity

#endif  // HEAP_H
