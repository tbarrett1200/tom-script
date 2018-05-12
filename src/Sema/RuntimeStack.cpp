#include "AST/Expr.h"
#include "Sema/RuntimeStack.h"
#include <memory>

RuntimeStack::RuntimeStack() {
  base_pointer = 0;
  push(std::make_shared<StackPointer>(0));
}

/* returns the number of expressions on the stack */
int RuntimeStack::size() const {
  return memory.size();
}

/*
 * Stores the given value as the operation result
 *
 * Params:
 * std::shared_ptr<Expr> e - the value to store
 */
void RuntimeStack::setResult(std::shared_ptr<Expr> e) {
  func_result = e;
}

/*
 * Retrieves the given value as the operation result
 *
 * Returns: the result of the most recent operation
 */
std::shared_ptr<Expr> RuntimeStack::getResult() const {
  return func_result;
}

int* RuntimeStack::getBase() {
  return &base_pointer;
}

void RuntimeStack::setBase(int e) {
  base_pointer = e;
}

void RuntimeStack::offset(int i) {
  for(int j=0; j<i; j++) {
    memory.pop_back();
  }
}

/*
 * Computes a memory address and returns the value stored at that address
 *
 * Param:
 * ComputedAddress a - the memory address to compute
 */
std::shared_ptr<Expr> RuntimeStack::get(ComputedAddress a) {
  return memory[a.get(*this)];
}

/*
 * Computes a memory address and stores the given value in that location
 *
 * Param:
 * ComputedAddress a - the memory address to compute
 * std::shared_ptr<Expr> e - the value to store
 */
void RuntimeStack::set(ComputedAddress a, std::shared_ptr<Expr> e) {
  memory[a.get(*this)] = e;
}

/*
 * returns the value stored at the given memory address
 *
 * Param:
 * int a - the memory address
 */
std::shared_ptr<Expr> RuntimeStack::get(int a) const {
  return memory[a];
}

/*
 * stores a given value in the given memory address
 *
 * Param:
 * int a - the memory address
 * std::shared_ptr<Expr> e - the value to store
 */
void RuntimeStack::set(int a, std::shared_ptr<Expr> e) {
  memory[a] = e;
}


/* pushes the given expression to the stack */
void RuntimeStack::push(std::shared_ptr<Expr> e) {
  memory.push_back(e);
}

std::shared_ptr<Expr> RuntimeStack::top() const {
  return memory.back();
}
/* returns expression on top of the stack, or null if stack is empty */
std::shared_ptr<Expr> RuntimeStack::pop() {
  if (memory.size() == 0) return nullptr;
  auto back = memory.back();
  memory.pop_back();
  return back;
}
