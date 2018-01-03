#include "Visitor.h"
#include "Tree.h"

IntegerLiteral::IntegerLiteral(string l)
  : lexeme{l} {}

void IntegerLiteral::accept(class Visitor &v) {
  return v.visit(this);
};

Identifier::Identifier(string l)
  : lexeme{l} {}

void Identifier::accept(class Visitor &v) {
  return v.visit(this);
};

Operator::Operator(string l)
  : lexeme{l} {}

void Operator::accept(class Visitor &v) {
  return v.visit(this);
};

BinaryExpr::BinaryExpr(Expr* l, Operator* o, Expr* r)
  : left{l}, op{o}, right{r} {}

void BinaryExpr::accept(class Visitor &v) {
  return v.visit(this);
};

StmtList::StmtList(Stmt* s, StmtList* n)
  : stmt{s}, next{n} {}

void StmtList::accept(class Visitor &v) {
  return v.visit(this);
}

BlockStmt::BlockStmt(StmtList* s)
  : stmts{s} {}

void BlockStmt::accept(class Visitor &v) {
  return v.visit(this);
};

VarDecl::VarDecl(Identifier* n, Identifier* t)
  : name{n}, type{t} {}

void VarDecl::accept(class Visitor &v) {
  return v.visit(this);
};

FuncDecl::FuncDecl(Identifier* n, StmtList* p, Identifier* r, BlockStmt* s)
  : name{n}, params{p}, retType{r}, stmt{s} {}

void FuncDecl::accept(class Visitor &v) {
  return v.visit(this);
};

IfStmt::IfStmt(Expr* c, BlockStmt* s)
  : cond{c}, stmt{s} {}

void IfStmt::accept(class Visitor &v) {
  return v.visit(this);
};

WhileStmt::WhileStmt(Expr* c,BlockStmt* s)
  : cond{c}, stmt{s} {}

void WhileStmt::accept(class Visitor &v) {
  return v.visit(this);
};

ExprStmt::ExprStmt(Expr* e)
  : expr{e} {}

void ExprStmt::accept(class Visitor &v) {
  return v.visit(this);
};

ReturnStmt::ReturnStmt(Expr* e)
  : expr{e} {}

void ReturnStmt::accept(class Visitor &v) {
  return v.visit(this);
};
