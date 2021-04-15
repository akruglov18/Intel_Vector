#include <cstddef>
#include <cassert>
#include <initializer_list>
#include<iostream>

template <typename T>
class vector {
public:
  vector() : my_capacity(0), my_size(0), my_data(nullptr) {}

  vector(const vector& vec) : my_capacity(vec.my_capacity)
    , my_size(vec.my_size)
    , my_data(static_cast<T*>(::operator new(sizeof(T) * my_capacity)))
  {
    for (std::size_t i = 0; i < my_size; ++i) {
      new(my_data + i) T(vec.my_data[i]);
    }
  }

  vector(vector&& other) noexcept : my_data(other.my_data)
    , my_size(other.my_size)
    , my_capacity(other.my_capacity)
  {
    other.my_data = nullptr;
    other.my_size = 0;
    other.my_capacity = 0;
  }

  vector(std::size_t size, const T& value) : my_capacity(size)
    , my_data(static_cast<T*>(::operator new(sizeof(T)* my_capacity)))
    , my_size(size)
  {
    for (std::size_t i = 0; i < size; i++)
      new (my_data + i) T(value);
  }

  vector(std::initializer_list<T> list) : my_capacity(list.size()), my_size(list.size())
    , my_data(static_cast<T*>(::operator new(sizeof(T) * my_capacity)))
  {
    std::size_t i = 0;
    for (auto it = list.begin(); it != list.end(); it++, i++) {
      new (my_data + i) T(*it);
    }
  }

  ~vector() {
    clear();
  }

  vector& operator=(const vector& vec) {
    if (this != &vec) {
      for (std::size_t i = 0; i < my_size; i++)
        my_data[i].~T();
      operator delete(my_data);
      my_capacity = vec.my_capacity;
      my_size = vec.my_size;
      my_data = static_cast<T*>(::operator new(sizeof(T) * my_capacity));
      for (std::size_t i = 0; i < my_size; ++i)
        new (my_data + i) T(vec.my_data[i]);
    }
    return *this;
  }

  vector& operator=(vector&& other) noexcept {
    if (this != &other) {
      for (std::size_t i = 0; i < my_size; i++)
        my_data[i].~T();
      operator delete (my_data);
      my_size = other.my_size;
      my_capacity = other.my_capacity;
      my_data = other.my_data;

      other.my_size = 0;
      other.my_capacity = 0;
      other.my_data = nullptr;
    }
    return *this;
  }

  T& operator[](std::size_t index) {
    assert(index < my_size);
    return my_data[index];
  }
  const T& operator[](std::size_t index) const {
    assert(index < my_size);
    return my_data[index];
  }

  void clear() {
    for (std::size_t i = 0; i < my_size; i++)
      my_data[i].~T();
    operator delete (my_data);
    my_data = nullptr;
    my_capacity = 0;
    my_size = 0;
  }

  void push_back(const T& v) {
    if (my_capacity == my_size) {
      if (my_capacity == 0) {
        my_capacity = 1;
      }
      else {
        my_capacity *= 2;
      }

      T* tmp = static_cast<T*>(::operator new(sizeof(T) * my_capacity));
      for (std::size_t i = 0; i < my_size; ++i) {
        new (tmp + i) T(std::move(my_data[i]));
        my_data[i].~T();
      }
      operator delete (my_data);

      my_data = tmp;
    }

    my_data[my_size] = v;
    ++my_size;
  }

  void pop_back() {
    if (my_size > 0) {
      my_size--;
      my_data[my_size].~T();
    }
  }

  void resize(std::size_t new_size, const T& val = T()) {
    if (new_size < my_size) {
      for (std::size_t i = new_size; i < my_size; ++i) {
        my_data[i].~T();
      }
    }
    else if (new_size > my_size) {
      if (new_size > my_capacity) {
        my_capacity = new_size;
        T* tmp = static_cast<T*>(::operator new(sizeof(T) * my_capacity));

        for (std::size_t i = 0; i < my_size; ++i) {
          new (tmp + i) T(std::move(my_data[i]));
          my_data[i].~T();
        }

        operator delete (my_data);
        my_data = tmp;
      }
      for (std::size_t i = my_size; i < new_size; ++i) {
        new(my_data + i) T(val);
      }
    }
    my_size = new_size;
  }
  void reserve(std::size_t capacity) {
    if (capacity > my_capacity) {
      my_capacity = capacity;
      T* new_data = static_cast<T*>(::operator new(sizeof(T) * my_capacity));
      for (std::size_t i = 0; i < my_size; i++) {
        new (new_data + i) T(std::move(my_data[i]));
        my_data[i].~T();
      }
      operator delete (my_data);
      my_data = new_data;
    }
  }

  std::size_t size() const {
    return my_size;
  }

private:
  std::size_t my_capacity;
  std::size_t my_size;
  T* my_data;
};

int main() {
  vector<int> vec_def(7, 3);
  for (int i = 0; i < vec_def.size(); i++) {
    std::cout << vec_def[i] << " ";
  }
  std::cout << std::endl;
  vector<int> vec{ 1, 2, 3, 4, 5, 6, 7 };
  for (int i = 0; i < vec.size(); i++) {
    std::cout << vec[i] << " ";
  }
  std::cout << std::endl;
  vector<int> vec_copy{ vec };
  for (int i = 0; i < vec_copy.size(); i++) {
    std::cout << vec_copy[i] << " ";
  }
  std::cout << std::endl;
  vec.pop_back();
  for (int i = 0; i < vec.size(); i++) {
    std::cout << vec[i] << " ";
  }
  std::cout << std::endl;
  vector<int> vec1 = std::move(vec);

  for (int i = 0; i < vec1.size(); i++) {
    std::cout << vec1[i] << " ";
  }
  std::cout << std::endl;

  vector<int> vec2{ 1, 2, 3, 4 };
  vec1 = std::move(vec2);
  for (int i = 0; i < vec1.size(); i++) {
    std::cout << vec1[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Resize" << std::endl;
  vector<int> vec_resize{ 1, 2, 3, 4, 5, 6, 7 };
  vec_resize.resize(3);
  for (int i = 0; i < vec_resize.size(); i++) {
    std::cout << vec_resize[i] << " ";
  }
  std::cout << std::endl;
  vec_resize.resize(6, 3);
  for (int i = 0; i < vec_resize.size(); i++) {
    std::cout << vec_resize[i] << " ";
  }
  std::cout << std::endl;
  vec_resize.resize(12, 3);
  for (int i = 0; i < vec_resize.size(); i++) {
    std::cout << vec_resize[i] << " ";
  }
  std::cout << std::endl;
}