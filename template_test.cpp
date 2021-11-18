#include <iostream>
#include <memory>
#include <string>
#include <vector>

class PItemBase {
 public:
  explicit PItemBase(std::string key) : key_(key) {}

  std::string Rep() { return key_ + " : " + value(); }

 private:
  virtual std::string value() = 0;
  std::string key_;
};

template <class T>
class NumericPItem : public PItemBase {
 public:
  NumericPItem(std::string key, T value)
      : PItemBase(std::move(key)), value_(value) {}

 private:
  virtual std::string value() override { return std::to_string(value_); }
  T value_;
};

class StringPItem : public PItemBase {
 public:
  StringPItem(std::string key, std::string str)
      : PItemBase(std::move(key)), str_(std::move(str)) {}

 private:
  virtual std::string value() override { return str_; }
  std::string str_;
};

template <class T>
class ClassPItem : public PItemBase {
 public:
  ClassPItem(std::string key, T t) : PItemBase(std::move(key)), t_(t) {}
  virtual std::string value() override { return t_.to_string(); }

 private:
  T t_;
};

template <class T, std::enable_if_t<
                       std::is_same<decltype(std::to_string(std::declval<T>())),
                                    std::string>::value,
                       bool> = true>
std::unique_ptr<PItemBase> MakePItem(std::string key, T value) {
  return std::unique_ptr<PItemBase>(new NumericPItem<T>(key, value));
}

std::unique_ptr<PItemBase> MakePItem(std::string key, std::string value) {
  return std::unique_ptr<PItemBase>(new StringPItem(key, value));
}

template <class T,
          std::enable_if_t<std::is_same<decltype(std::declval<T>().to_string()),
                                        std::string>::value,
                           bool> = true>
std::unique_ptr<PItemBase> MakePItem(std::string key, T value) {
  return std::unique_ptr<PItemBase>(new ClassPItem<T>(key, value));
}

class DemoClass {
 public:
  DemoClass() = default;
  std::string to_string() { return "DemoClass"; }
};

typedef std::unique_ptr<PItemBase> PItemPtr;

int main() {
  std::vector<PItemPtr> vp;
  vp.push_back(MakePItem("world", "test"));
  vp.push_back(MakePItem("hell", std::string("test")));
  vp.push_back(MakePItem("Demo", DemoClass()));
  vp.push_back(MakePItem("hello", 1));
  for (auto&& e : vp) {
    std::cout << e->Rep() << std::endl;
  }

  return 0;
}
