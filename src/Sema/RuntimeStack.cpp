#include "AST/Expr.h"
#include "Sema/RuntimeStack.h"

RuntimeStack::RuntimeStack() {
  base_pointer = std::make_shared<StackReference>(0);
  push(base_pointer);
}

void RuntimeStack::setResult(std::shared_ptr<Expr> e) {
  func_result = e;
}

std::shared_ptr<Expr> RuntimeStack::getResult() const {
  return func_result;
}

/* pushes the given expression to the stack */
void RuntimeStack::push(std::shared_ptr<Expr> e) {
  memory.push_back(e);
}

std::shared_ptr<StackReference> RuntimeStack::getBase() const {
  return base_pointer;
}
void RuntimeStack::setBase(std::shared_ptr<StackReference> e) {
  base_pointer = e;
}

/* returns expression on top of the stack, or null if stack is empty */
std::shared_ptr<Expr> RuntimeStack::pop() {
  if (memory.size() == 0) return nullptr;
  auto back = memory.back();
  memory.pop_back();
  return back;
}

/* returns the number of expressions on the stack */
int RuntimeStack::size() const {
  return memory.size();
}

/* offsets the stack pointer by a given*/
void RuntimeStack::offset(int offset) {
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

std::shared_ptr<Expr> RuntimeStack::RuntimeStack::top() const {
  return memory.back();
}

std::shared_ptr<Expr> RuntimeStack::top(int i) const {
  return memory[memory.size()-1-i];
}

std::shared_ptr<Expr> RuntimeStack::param(int i) const {
  return memory[base_pointer->location-i];
}

std::shared_ptr<Expr> RuntimeStack::at(int i) const {
  return memory[i];
}


std::shared_ptr<Expr> RuntimeStack::get(std::shared_ptr<StackReference> o) const {
  return memory[base_pointer->location+o->location];
}

std::shared_ptr<Expr> RuntimeStack::get(int i) const {
  return memory[base_pointer->location+i];
}

void RuntimeStack::set(std::shared_ptr<StackReference> o, std::shared_ptr<Expr> e) {
  memory[o->location] = e;
}
