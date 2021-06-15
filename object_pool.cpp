#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

using namespace std;

const int MaxObjectNum = 10;

template <typename T> class ObjectPool {
  template <typename... Args>
  using Constructor = std::function<std::shared_ptr<T>(Args...)>;

public:
  template <typename... Args> void Init(size_t num, Args &&... args) {
    if (num <= 0 || num > MaxObjectNum)
      throw std::logic_error("object num out of range");

    auto constructName = typeid(Constructor<Args...>).name();
    for (size_t i = 0; i < num; i++) {
      m_objectMap.emplace(constructName,
                          shared_ptr<T>(new T(std::forward<Args>(args)...),
                                        [this, constructName](T *p) {
                                          m_objectMap.emplace(
                                              std::move(constructName),
                                              std::shared_ptr<T>(p));
                                        }));
    }
  }

  template <typename... Args> shared_ptr<T> Get() {
    string constructName = typeid(Constructor<Args...>).name();
    auto range = m_objectMap.equal_range(constructName);
    for (auto it = range.first; it != range.second; ++it) {
      auto ptr = it->second;
      m_objectMap.erase(it);
      return ptr;
    }
    return nullptr;
  }

private:
  std::multimap<string, std::shared_ptr<T>> m_objectMap;
};

struct BigObject {
  BigObject() {}
  BigObject(int a) {}
  BigObject(const int &a, const int &b) {}
  void Print(const string &str) { std::cout << str << std::endl; }
};

void Print(shared_ptr<BigObject> p, const string &str) {
  if (p != nullptr) {
    p->Print(str);
  }
}

void TestObjPool() {
  ObjectPool<BigObject> pool;
  pool.Init(2);
  {
    auto p = pool.Get();
    Print(p, "p");
    auto p2 = pool.Get();
    Print(p2, "p2");
  }

  auto p = pool.Get();
  auto p2 = pool.Get();
  Print(p, "p");
  Print(p2, "p2");

  pool.Init(2, 1);
  auto p4 = pool.Get<int>();
  Print(p4, "p4");
  pool.Init(2, 1, 2);
  auto p5 = pool.Get<int, int>();
  Print(p5, "p5");
}

int main() {
  TestObjPool();
  return 0;
}
