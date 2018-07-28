#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/Expr.h"


//----------------------------------------------------------------------------//
// CompoundStmt
//----------------------------------------------------------------------------//


CompoundStmt::CompoundStmt(std::vector<std::shared_ptr<Stmt>>&& l) : list{std::move(l)} {}

Stmt::Kind CompoundStmt::getKind() const { return Kind::CompoundStmt;}

bool CompoundStmt::returns() const {
  for (auto stmt: list) {
    if (stmt->returns()) return true;
  }
  return false;
}

std::vector<std::shared_ptr<TreeElement>> CompoundStmt::getChildren() const {
  std::vector<std::shared_ptr<TreeElement>> treeVector;
  std::copy(list.begin(), list.end(), std::back_inserter(treeVector));
  return treeVector;
}

//----------------------------------------------------------------------------//
// ConditionalStmt
//----------------------------------------------------------------------------//


ConditionalStmt::ConditionalStmt(std::shared_ptr<Expr> c, std::shared_ptr<CompoundStmt> s) : condition{c}, stmt{s} {
  if (!s) throw std::logic_error("violated precondition: statement is required");
}

Stmt::Kind ConditionalStmt::getKind() const { return Kind::ConditionalStmt;}

bool ConditionalStmt::returns() const {
  return stmt->returns();
}

bool ConditionalStmt::isElseStmt() const {
  return !condition;
}

std::vector<std::shared_ptr<TreeElement>> ConditionalStmt::getChildren() const {
  if (!condition) { return {stmt}; }
  else return {condition, stmt};
}

//----------------------------------------------------------------------------//
// ConditionalStmtList
//----------------------------------------------------------------------------//

ConditionalStmtList::ConditionalStmtList(std::shared_ptr<ConditionalStmt> e, std::shared_ptr<ConditionalStmtList> l): element{e}, list{l} {
  if (!e) throw std::logic_error("violated precondition: statement is required");
  if (e->isElseStmt() && list) throw std::logic_error("violated precondition: else statement must be last statement in list");
}

ConditionalStmtList::ConditionalStmtList(std::vector<std::shared_ptr<ConditionalStmt>> l) {
  if (l.size() == 0) {
    throw std::runtime_error("type list must have at least one type");
  }
  if (l.size() == 1) {
    element = l[0];
    list = nullptr;
  } else {
    element = l[0];
    l.erase(l.begin());
    list = std::make_shared<ConditionalStmtList>(l);
  }
}

bool ConditionalStmtList::returns() const {
  if (!hasElseStmt()) return false;
  else {
    if (list == nullptr) return element->returns();
    else return element->returns() && list->returns();
  }
}


Stmt::Kind ConditionalStmtList::getKind() const { return Kind::ConditionalStmtList; }

std::vector<std::shared_ptr<TreeElement>> ConditionalStmtList::getChildren() const {
  if (!list) return {element};
  else {
    auto children = list->getChildren();
    children.insert(children.begin(), element);
    return children;
  }
}

int ConditionalStmtList::size() const {
  if (!list) return 1;
  else return list->size()+1;
}

bool ConditionalStmtList::hasElseStmt() const {
  if (list) return list->hasElseStmt();
  else return element->isElseStmt();
}

//----------------------------------------------------------------------------//
// WhileLoop
//----------------------------------------------------------------------------//

WhileLoop::WhileLoop(std::shared_ptr<Expr> c, std::shared_ptr<CompoundStmt> s): condition{c}, stmt{s} {
  if (!c) throw std::logic_error("violated precondition: condition is required");
  if (!s) throw std::logic_error("violated precondition: statement is required");
}

bool WhileLoop::returns() const {
  return stmt->returns();
}

Stmt::Kind WhileLoop::getKind() const { return Kind::WhileLoop;}

std::vector<std::shared_ptr<TreeElement>> WhileLoop::getChildren() const {
  return {condition, stmt};
}



//----------------------------------------------------------------------------//
// ReturnStmt
//----------------------------------------------------------------------------//

ReturnStmt::ReturnStmt(std::shared_ptr<Expr> d): expr{d} {}

bool ReturnStmt::returns() const {
  return true;
}

Stmt::Kind ReturnStmt::getKind() const { return Kind::ReturnStmt;}

std::vector<std::shared_ptr<TreeElement>> ReturnStmt::getChildren() const {
  if (expr) return {expr};
  else return {};
}

//----------------------------------------------------------------------------//
// ExprStmt
//----------------------------------------------------------------------------//

ExprStmt::ExprStmt(std::shared_ptr<Expr> e): expr{e} {
  if (!e) throw std::logic_error("violated precondition: expr is required");
}

bool ExprStmt::returns() const {
  return false;
}

Stmt::Kind ExprStmt::getKind() const { return Kind::ExprStmt;}

std::vector<std::shared_ptr<TreeElement>> ExprStmt::getChildren() const {
  return {expr};
}


//----------------------------------------------------------------------------//
// DeclStmt
//----------------------------------------------------------------------------//

DeclStmt::DeclStmt(std::shared_ptr<Decl> d): decl{d} {
  if (!d) throw std::logic_error("violated precondition: decl is required");
}

Stmt::Kind DeclStmt::getKind() const { return Kind::DeclStmt;}

std::vector<std::shared_ptr<TreeElement>> DeclStmt::getChildren() const {
  return {decl};
}

bool DeclStmt::returns() const {
  return false;
}
