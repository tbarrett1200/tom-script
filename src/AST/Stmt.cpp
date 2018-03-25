#include "AST/Stmt.h"

//----------------------------------------------------------------------------//
// StmtList
//----------------------------------------------------------------------------//

template <typename T> bool StmtList::has() const {
  return dynamic_pointer_cast<T>(element) || list ? list->has<T>() : false;
}

StmtList::StmtList(std::shared_ptr<Stmt> e, std::shared_ptr<StmtList> l)
  : element{move(e)}, list{move(l)} {}

StmtList::StmtList(std::vector<std::shared_ptr<Stmt>> l) {
  if (l.size() == 0) {
    throw std::runtime_error("type list must have at least one type");
  }
  if (l.size() == 1) {
    element = l[0];
    list = nullptr;
  } else {
    element = l[0];
    l.erase(l.begin());
    list = std::make_shared<StmtList>(l);
  }
}

bool StmtList::returns() const {
  if (element->returns()) return true;
  else if (list == nullptr) return false;
  else return list->returns();
}

std::vector<std::shared_ptr<Matchable>> StmtList::getChildren() const {
  if (!list) return {element};
      else {
    auto children = list->getChildren();
    children.insert(children.begin(), element);
    return children;
  }
}

int StmtList::size() const {
  if (!list) return 1;
  else return list->size()+1;
}

//----------------------------------------------------------------------------//
// CompoundStmt
//----------------------------------------------------------------------------//


CompoundStmt::CompoundStmt(std::shared_ptr<StmtList> l) : list{l} {}

Stmt::Kind CompoundStmt::getKind() const { return Kind::CompoundStmt;}

bool CompoundStmt::returns() const {
  return list ? list->returns() : false;
}

std::vector<std::shared_ptr<Matchable>> CompoundStmt::getChildren() const {
  return {list};
}

//----------------------------------------------------------------------------//
// ConditionalStmt
//----------------------------------------------------------------------------//


ConditionalStmt::ConditionalStmt(std::shared_ptr<Expr> c, std::shared_ptr<CompoundStmt> s) : condition{c}, stmt{s} {
  if (!s) throw logic_error("violated precondition: statement is required");
}

Stmt::Kind ConditionalStmt::getKind() const { return Kind::ConditionalStmt;}

bool ConditionalStmt::returns() const {
  return stmt->returns();
}

bool ConditionalStmt::isElseStmt() const {
  return !condition;
}

std::vector<std::shared_ptr<Matchable>> ConditionalStmt::getChildren() const {
  if (!condition) { return {stmt}; }
  else return {condition, stmt};
}

//----------------------------------------------------------------------------//
// ConditionalStmtList
//----------------------------------------------------------------------------//

ConditionalStmtList::ConditionalStmtList(std::shared_ptr<ConditionalStmt> e, std::shared_ptr<ConditionalStmtList> l): element{e}, list{l} {
  if (!e) throw logic_error("violated precondition: statement is required");
  if (e->isElseStmt() && list) throw logic_error("violated precondition: else statement must be last statement in list");
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

std::vector<std::shared_ptr<Matchable>> ConditionalStmtList::getChildren() const {
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
  if (!c) throw logic_error("violated precondition: condition is required");
  if (!s) throw logic_error("violated precondition: statement is required");
}

bool WhileLoop::returns() const {
  return stmt->returns();
}

Stmt::Kind WhileLoop::getKind() const { return Kind::WhileLoop;}

std::vector<std::shared_ptr<Matchable>> WhileLoop::getChildren() const {
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

std::vector<std::shared_ptr<Matchable>> ReturnStmt::getChildren() const {
  return {expr};
}

//----------------------------------------------------------------------------//
// ExprStmt
//----------------------------------------------------------------------------//

ExprStmt::ExprStmt(std::shared_ptr<Expr> e): expr{e} {
  if (!e) throw logic_error("violated precondition: expr is required");
}

bool ExprStmt::returns() const {
  return false;
}

Stmt::Kind ExprStmt::getKind() const { return Kind::ExprStmt;}

std::vector<std::shared_ptr<Matchable>> ExprStmt::getChildren() const {
  return {expr};
}


//----------------------------------------------------------------------------//
// DeclStmt
//----------------------------------------------------------------------------//

DeclStmt::DeclStmt(std::shared_ptr<Decl> d): decl{d} {
  if (!d) throw logic_error("violated precondition: decl is required");
}

Stmt::Kind DeclStmt::getKind() const { return Kind::DeclStmt;}

std::vector<std::shared_ptr<Matchable>> DeclStmt::getChildren() const {
  return {decl};
}

bool DeclStmt::returns() const {
  return false;
}
