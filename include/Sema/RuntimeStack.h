#ifndef SEMA_STACK_H
#define SEMA_STACK_H

#include <memory>
#include <vector>

class Expr;
class StackReference;

class RuntimeStack {
private:
  // the runtime stack, which stores all local variables and temporary values
  std::vector<std::shared_ptr<Expr>> memory;
  // the returned value from a function
  std::shared_ptr<Expr> func_result;
  // the location of the stack base
  std::shared_ptr<StackReference> base_pointer;

public:
  RuntimeStack();
  void push(std::shared_ptr<Expr> e);
  std::shared_ptr<Expr> pop();

  int size() const;

  std::shared_ptr<Expr> getResult() const;
  void setResult(std::shared_ptr<Expr> e);

  std::shared_ptr<StackReference> getBase() const;
  void setBase(std::shared_ptr<StackReference> e);

  std::shared_ptr<Expr> top() const;
  std::shared_ptr<Expr> top(int i) const;
  std::shared_ptr<Expr> at(int i) const;
  std::shared_ptr<Expr> get(std::shared_ptr<StackReference> o) const;
  std::shared_ptr<Expr> get(int i) const;
  std::shared_ptr<Expr> param(int i) const;

  void offset(int offset);
  void set(std::shared_ptr<StackReference>, std::shared_ptr<Expr>);

};
#endif
