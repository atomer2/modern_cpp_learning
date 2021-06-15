#include <functional>
#include <iostream>
#include <type_traits>

using namespace std;

template <typename R = void> struct CommCommand {
private:
  std::function<R()> m_f;

public:
  // SNIFE
  template <class F, class... Args,
            class = typename std::enable_if<
                !std::is_member_function_pointer<F>::value>::type>
  void Wrap(F &&f, Args &&... args) {
    m_f = [&] { return f(args...); };
  }

  template <class RT, class C, class... DArgs, class P, class... Args>
  void Wrap(RT (C::*f)(DArgs...) const, P &&p, Args &&... args) {
    m_f = [&, f] { return (*p.*f)(args...); };
  }

  template <class RT, class C, class... DArgs, class P, class... Args>
  void Wrap(RT (C::*f)(DArgs...), P &&p, Args &&... args) {
    m_f = [&, f] { return (*p.*f)(args...); };
  }

  R Execute() { return m_f(); }
};

struct STA {
  int m_a;
  int operator()() { return m_a; }
  int operator()(int n) { return m_a + n; }
  int triple0() { return m_a * 3; }
  int triple(int a) { return m_a * 3 + a; }
  int triple1() const { return m_a * 3; }
  const int triple2(int a) const { return m_a * 3 + a; }
  void triple3() {cout << "" << endl;}
};

int add_one(int n){
  return n + 1;
}

int main() {
  CommCommand<int> cmd;
  cmd.Wrap(add_one, 0);
  cmd.Wrap([](int n){return n+1;}, 1);

  STA t = {10};
  int x = 3;
  cmd.Wrap(&STA::triple0, &t);
  cmd.Wrap(&STA::triple, &t, x);
  cmd.Wrap(&STA::triple, &t, 3);

  auto r = cmd.Execute();
  CommCommand<> cmd1;
  return 0;
}
