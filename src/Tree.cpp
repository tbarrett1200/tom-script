#include "Visitor.h"
#include "Tree.h"

IntLiteral::IntLiteral(Token l)
  : token{l} {}

void IntLiteral::accept(class Visitor &v) {
  return v.visit(this);
};

DoubleLiteral::DoubleLiteral(Token l)
  : token{l} {}

void DoubleLiteral::accept(class Visitor &v) {
  return v.visit(this);
};

StringLiteral::StringLiteral(Token l)
  : token{l} {}

void StringLiteral::accept(class Visitor &v) {
  return v.visit(this);
};

Identifier::Identifier(Token l)
  : token{l} {}

void Identifier::accept(class Visitor &v) {
  return v.visit(this);
};

Type::Type(Token l)
  : token{l} {}

void Type::accept(class Visitor &v) {
  return v.visit(this);
};

Operator::Operator(Token l)
  : token{l} {}

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

VarDecl::VarDecl(Identifier* n, Type* t)
  : name{n}, type{t} {}

void VarDecl::accept(class Visitor &v) {
  return v.visit(this);
};

FuncDecl::FuncDecl(Identifier* n, StmtList* p, Type* r, BlockStmt* s)
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

Program::Program(StmtList* s) : block{new BlockStmt(s)}{}

void Program::accept(class Visitor &v) {
  return v.visit(this);
};
