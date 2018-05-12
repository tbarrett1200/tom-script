#ifndef SEMA_STACK_H
#define SEMA_STACK_H

#include "Sema/MemoryLoc.h"

#include <memory>
#include <vector>

class Expr;
class StackPointer;

class RuntimeStack : public RegisterDelegate {
private:
  // the runtime stack, which stores all local variables and temporary values
  std::vector<std::shared_ptr<Expr>> memory;
  // the returned value from a function
  std::shared_ptr<Expr> func_result;
  // the location of the stack base
  int base_pointer;

public:
  RuntimeStack();

  int* get(Register r) {
    switch (r) {
    case Register::BASE_POINTER: return &base_pointer;
    default: return nullptr;
    }
  }

  void push(std::shared_ptr<Expr> e);
  std::shared_ptr<Expr> pop();
  std::shared_ptr<Expr> top() const;

  int size() const;

  std::shared_ptr<Expr> getResult() const;
  void setResult(std::shared_ptr<Expr> e);

  int* getBase();
  void setBase(int e);
  void offset(int i);

  std::shared_ptr<Expr> get(ComputedAddress);
  void set(ComputedAddress, std::shared_ptr<Expr>);

  std::shared_ptr<Expr> get(int) const;
  void set(int, std::shared_ptr<Expr>);
};
#endif
