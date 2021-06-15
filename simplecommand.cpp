#include <iostream>
using namespace std;

class Command {
public:
  void Execute();
};

template <typename Receiver> class SimpleCommand : public Command {
public:
  typedef void (Receiver::*Action)();
  SimpleCommand(Receiver *r, Action a) : _receiver(r), _action(a) {}
  virtual void Execute();

private:
  Action _action;
  Receiver *_receiver;
};

template <class Receiver> void SimpleCommand<Receiver>::Execute() {
  (_receiver->*_action)();
}

class MyClass {
public:
  void Action();
};

void dummy() {
  MyClass *receiver = new MyClass;
  Command *aCommand = new SimpleCommand<MyClass>(receiver, &MyClass::Action);
  aCommand->Execute();
}
