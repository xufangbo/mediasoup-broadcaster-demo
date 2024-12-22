#include <iostream>

template <class T>
class MyRef {
 private:
  T obj;

 public:
  void fun() {
    obj.test();
  }
};

class MyClass {
 public:
  void test() {
    std::cout << "hello" << std::endl;
  }
};

int main() {
  MyRef<MyClass> xx;
  xx.fun();
  return 0;
}