#include <iostream>
#include <type_traits>

//_Pragma("pack(1)")
struct MyStruct{
  char a;
  int b;
  short c;
  long long d;
  char e;
};
//_Pragma("pack()")

int main(){
  std::cout << "sizeof MyStruct is " << sizeof(MyStruct)<< std::endl;
  std::cout << "alignment of MyStruct is " <<  std::alignment_of<MyStruct>::value << std::endl;
  return 0;
}
