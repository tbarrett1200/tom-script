#include "AST/Expr.h"
#include "Sema/RuntimeStack.h"

std::shared_ptr<Expr> RuntimeStack::get(std::shared_ptr<StackReference> o) {
  return memory[o->location];
}

void RuntimeStack::set(std::shared_ptr<StackReference> o, std::shared_ptr<Expr> e) {
  memory[o->location] = e;
}
