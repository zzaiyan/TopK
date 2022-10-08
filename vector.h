#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm>

#define TT template <typename T>
#define vec Vector<T>

constexpr int DEFAULT_SIZE = 5;

TT class Vector {
 protected:
  T* _data;
  int _size, _capacity;

  void _init(int cp = DEFAULT_SIZE, int sz = 0) {
    _data = new T[_capacity = cp], _size = sz;
  }

 public:
  Vector() { _init(); }
  Vector(T* arr, int n);
  ~Vector();
  int size() { return _size; }
  int capacity() { return _capacity; }
  bool empty() { return _size == 0; }
  T& operator[](int r);
  T& front() { return _data[0]; }
  T& back() { return _data[_size - 1]; }
  bool reserve(int s);
  bool shrink();
  void remove(int r);
  int remove(int l, int r);
  void insert(int r, const T& e);
  void clear();
  void push_back(const T& e);
  void pop_back();
};

TT vec::Vector(T* arr, int n) {
  _init(std::max(DEFAULT_SIZE, n * 2), n);
  for (int i = 0; i < n; i++)
    _data[i] = arr[i];
}

TT vec::~Vector() {
  if (_data)
    delete[] _data;
}

TT T& vec::operator[](int r) {
  return _data[std::max(0, r)];
}

TT bool vec::reserve(int s) {
  if (s <= _size)
    return false;
  T* newSpace = new T[_capacity = s];
  for (int i = 0; i < _size; i++)
    newSpace[i] = _data[i];
  delete[] _data;
  _data = newSpace;
  return true;
}

TT bool vec::shrink() {
  if (_capacity == _size)
    return false;
  T* newSpace = new T[_capacity = _size];
  for (int i = 0; i < _size; i++)
    newSpace[i] = _data[i];
  delete[] _data;
  _data = newSpace;
  return true;
}

TT void vec::remove(int r) {
  for (int i = r; i + 1 < _size; i++)
    _data[i] = _data[i + 1];
  _size--;
}

TT int vec::remove(int l, int r) {
  int len = r - l;
  for (int i = l; i + len < _size; i++)
    _data[i] = _data[i + len];
  _size -= len;
  return len;
}

TT void vec::insert(int r, const T& e) {
  if (_capacity < _size + 1)
    reserve(_size * 2);
  for (int i = _size - 1; i >= r; i--)
    _data[i + 1] = _data[i];
  _data[r] = e;
  _size++;
}

TT void vec::clear() {
  delete[] _data;
  _data = new T[_capacity = DEFAULT_SIZE];
  _size = 0;
}

TT void vec::push_back(const T& e) {
  if (_capacity < _size + 1)
    reserve(_size * 2);
  _data[_size++] = e;
}

TT void vec::pop_back() {
  if (_size > 0)
    _size--;
}

#undef TT
#undef vec

#endif  // VECTOR_H
