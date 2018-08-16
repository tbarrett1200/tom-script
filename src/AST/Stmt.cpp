#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/Expr.h"

std::vector<std::shared_ptr<TreeElement>> CompilationUnit::getChildren() const {
  std::vector<std::shared_ptr<TreeElement>> children;
  for (auto stmt: stmts) {
    children.push_back(stmt);
  }
  return children;
}

std::vector<std::shared_ptr<TreeElement>> DeclStmt::getChildren() const {
  return {decl_};
}

std::vector<std::shared_ptr<TreeElement>> ExprStmt::getChildren() const {
  return {expr_};
}

std::vector<std::shared_ptr<TreeElement>> ReturnStmt::getChildren() const {
  if (expr_)
    return { expr_ };
  else return {};
}

std::vector<std::shared_ptr<TreeElement>> WhileLoop::getChildren() const {
  return {condition_, stmt_};
}

std::vector<std::shared_ptr<TreeElement>> ConditionalStmt::getChildren() const {
  if (!condition) { return {stmt}; }
  else return {condition, stmt};
}
