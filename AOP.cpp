#include <functional>
#include <iostream>

using namespace std;

#define HAS_MEMBER(member)                                                     \
  template <typename T, typename... Args> struct has_member_##member {         \
  private:                                                                     \
    template <typename U>                                                      \
    static auto Check(int)                                                     \
        -> decltype(std::declval<U>().member(std::declval<Args>()...),         \
                    std::true_type());                                         \
    template <typename U> static std::false_type Check(...);                   \
                                                                               \
  public:                                                                      \
    enum {                                                                     \
      value = std::is_same<decltype(Check<T>(0)), std::true_type>::value       \
    };                                                                         \
  };

HAS_MEMBER(foo)
HAS_MEMBER(Before)
HAS_MEMBER(After)

template <typename Func, typename... Args> class Aspect {
public:
  Aspect(Func &&f) : m_func(std::forward<Func>(f)) {}
  template <typename T>
  typename std::enable_if<has_member_Before<T, Args...>::value &&
                          has_member_After<T, Args...>::value>::type
  Invoke(Args &&... args, T &&aspect) {
    aspect.Before(std::forward<Args>(args)...);
    m_func(std::forward<Args>(args)...);
    aspect.After(std::forward<Args>(args)...);
  }

  template <typename T>
  typename std::enable_if<has_member_Before<T, Args...>::value &&
                          !has_member_After<T, Args...>::value>::type
  Invoke(Args &&... args, T &&aspect) {
    aspect.Before(std::forward<Args>(args)...);
    m_func(std::forward<Args>(args)...);
  }

  template <typename T>
  typename std::enable_if<!has_member_Before<T, Args...>::value &&
                          has_member_After<T, Args...>::value>::type
  Invoke(Args &&... args, T &&aspect) {
    m_func(std::forward<Args>(args)...);
    aspect.After(std::forward<Args>(args)...);
  }

  // Not perfect.
  template <typename Head, typename... Tail>
  void Invoke(Args &&... args, Head &&headAspect, Tail &&... tailAspect) {
    headAspect.Before(std::forward<Args>(args)...);
    Invoke(std::forward<Args>(args)..., std::forward<Tail>(tailAspect)...);
    headAspect.After(std::forward<Args>(args)...);
  }

private:
  Func m_func;
};

// Helper function.
template <typename... AP, typename... Args, typename Func>
void Invoke(Func &&f, Args &&... args) {
  Aspect<Func, Args...> asp(std::forward<Func>(f));
  asp.Invoke(std::forward<Args>(args)..., AP()...);
}

// test
struct AA {
  void Before(int i) { cout << "Before from AA" << i << endl; }
  void After(int i) { cout << "After from AA" << i << endl; }
};

struct BB {
  void Before(int i) { cout << "Before from BB" << i << endl; }
  void After(int i) { cout << "After from BB" << i << endl; }
};

struct CC {
  void Before() { cout << "Before from CC" << endl; }
  void After() { cout << "After from CC" << endl; }
};

struct DD {
  void Before() { cout << "Before from DD" << endl; }
  void After() { cout << "After from DD" << endl; }
};

void GT() { cout << "real GT function" << endl; }
void HT(int a) { cout << "real HT function:" << a << endl; }

void TestAop() {
  std::function<void(int)> f = std::bind(&HT, std::placeholders::_1);
  Invoke<AA, BB>(f, 1);

  Invoke<CC, DD>(&GT);
  Invoke<AA, BB>(&HT, 1);

  Invoke<AA, BB>([](int i) {}, 1);
  Invoke<CC, DD>([] {});
}

int main() {
  TestAop();
  return 0;
}
