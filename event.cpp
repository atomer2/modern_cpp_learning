#include <functional>
#include <iostream>
#include <map>
#include <string>

using namespace std;

class NonCopyable {
protected:
  NonCopyable() = default;
  ~NonCopyable() = default;
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
};

template <typename Func> class Events : NonCopyable {
public:
  Events() {}
  ~Events() {}
  int Connect(Func &&f) { return Assign(f); }

  int Connect(const Func &f) { return Assign(f); }

  void Disconnect(int key) { m_connections.earse(key); }

  template <typename... Args> void Notify(Args &&... args) {
    for (auto &it : m_connections) {
      it.second(std::forward<Args>(args)...);
    }
  }

private:
  template <typename F> int Assign(F &&f) {
    int k = m_observerId++;
    m_connections.emplace(k, std::forward<F>(f));
    return k;
  }

  int m_observerId = 0;
  std::map<int, Func> m_connections;
};

struct stA {
  int a, b;
  void print(int a, int b) { cout << a << ", " << b << endl; }
};

void print(int a, int b) { cout << a << ", " << b << endl; }

int main() {
  Events<std::function<void(int, int)>> myevent;

  auto key = myevent.Connect(print);
  stA t;
  auto lambdakey = myevent.Connect([&t](int a, int b) {
    t.a = a;
    t.b = b;
  });

  function<void(int, int)> f =
      std::bind(&stA::print, &t, std::placeholders::_1, std::placeholders::_2);

  myevent.Connect(f);
  int a = 1, b = 2;
  myevent.Notify(a, b);

  return 0;
}
