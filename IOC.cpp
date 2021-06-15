#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

using namespace std;

template <class T> class IocContainer {
public:
  IocContainer(void) {}
  ~IocContainer(void) {}

  template <class Drived> void RegisterType(string strKey) {
    std::function<T *()> function = [] { return new Drived(); };
    RegisterType(strKey, function);
  }

  T *Resolve(string strKey) {
    if (m_creatorMap.find(strKey) == m_creatorMap.end())
      return nullptr;
    std::function<T *()> function = m_creatorMap[strKey];
    return function();
  }

  std::shared_ptr<T> ResolveShared(string strKey) {
    T *ptr = Resolve(strKey);
    return std::shared_ptr<T>(ptr);
  }

private:
  void RegisterType(string strKey, std::function<T *()> creator) {
    if (m_creatorMap.find(strKey) != m_creatorMap.end()) {
      throw std::invalid_argument("this key has already exist!");
    }
    m_creatorMap.emplace(strKey, creator);
  }

private:
  map<string, std::function<T *()>> m_creatorMap;
};
