#ifndef SEMA_STACK_H
#define SEMA_STACK_H

#include <memory>
#include <vector>

class Expr;
class StackReference;

class RuntimeStack {
private:
  std::vector<std::shared_ptr<Expr>> memory;
  std::vector<std::shared_ptr<Expr>> temp;
  std::shared_ptr<StackReference> base;

public:

  void pushTemp(std::shared_ptr<Expr> e) {
    temp.push_back(e);
  }

  std::shared_ptr<Expr> popTemp() {
    if (temp.size() > 0) {
      auto ret = temp.back();
      temp.pop_back();
      return ret;
    } else return nullptr;
  }

  void push(std::shared_ptr<Expr> e) {
    memory.push_back(e);
  }

  std::shared_ptr<Expr> pop() {
    if (memory.size() > 0) {
      auto ret = memory.back();
      memory.pop_back();
      return ret;
    } else return nullptr;
  }

  int size() const {
    return memory.size();
  }
  void move(int offset) {
    if (offset>0) {
      for (int i = 0; i < offset; i++) {
        memory.push_back(nullptr);
      }
    } else if (offset<0) {
      for (int i = 0; i < -offset; i++) {
        memory.pop_back();
      }
    }
  }

  std::shared_ptr<Expr> top() {
    return memory.back();
  }

  std::shared_ptr<Expr> top(int i) {
    return memory[memory.size()-1-i];
  }

  std::shared_ptr<Expr> get(std::shared_ptr<StackReference> o);
  void set(std::shared_ptr<StackReference>, std::shared_ptr<Expr>);

};
#endif
