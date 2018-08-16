#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/Expr.h"

std::vector<TreeElement*> CompilationUnit::getChildren() const {
  std::vector<TreeElement*> children;
  for (auto &stmt: stmts_) {
    children.push_back(stmt.get());
  }
  return children;
}

std::vector<TreeElement*> DeclStmt::getChildren() const {
  return {decl_.get()};
}

std::vector<TreeElement*> ExprStmt::getChildren() const {
  return {expr_.get()};
}

std::vector<TreeElement*> ReturnStmt::getChildren() const {
  if (expr_)
    return { expr_.get() };
  else return {};
}

std::vector<TreeElement*> WhileLoop::getChildren() const {
  return { condition_.get(), stmt_.get()};
}

std::vector<TreeElement*> ConditionalStmt::getChildren() const {
  if (!condition) { return { stmt.get()}; }
  else return { condition.get(), stmt.get()};
}
