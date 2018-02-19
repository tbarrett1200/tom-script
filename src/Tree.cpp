#include "Visitor.h"
#include "Tree.h"


IntLiteral::IntLiteral(Token l) : token{l} {}

void IntLiteral::accept(class Visitor &v) {
  return v.visit(this);
};

DoubleLiteral::DoubleLiteral(Token l) : token{l} {}

void DoubleLiteral::accept(class Visitor &v) {
  return v.visit(this);
};

StringLiteral::StringLiteral(Token l) : token{l} {}

void StringLiteral::accept(class Visitor &v) {
  return v.visit(this);
};

Identifier::Identifier(Token l) : token{l} {}

void Identifier::accept(class Visitor &v) {
  return v.visit(this);
};

Type::Type(Token l) : token{l} {}

void Type::accept(class Visitor &v) {
  return v.visit(this);
};

OperatorNode::OperatorNode(Token l) : token{l}, op{l.lexeme} {}

void OperatorNode::accept(class Visitor &v) {
  return v.visit(this);
};

BinaryExpr::BinaryExpr(Expr* l, OperatorNode* o, Expr* r) : left{l}, op{o}, right{r} {
  l->parent = this;
  o->parent = this;
  r->parent = this;
}

void BinaryExpr::accept(class Visitor &v) {
  return v.visit(this);
};

UnaryExpr::UnaryExpr(Expr* e, OperatorNode* o) : op{o}, expr{e} {
  e->parent = this;
  o->parent = this;
}

void UnaryExpr::accept(class Visitor &v) {
  return v.visit(this);
};

StmtList::StmtList(Stmt* s, StmtList* n) : stmt{s}, next{n} {
  s->parent = this;
  n->parent = this;
}

void StmtList::accept(class Visitor &v) {
  return v.visit(this);
}

ExprList::ExprList(Expr* s, ExprList* n) : stmt{s}, next{n} {
  s->parent = this;
  n->parent = this;
}

void ExprList::accept(class Visitor &v) {
  return v.visit(this);
}

BlockStmt::BlockStmt(StmtList* s): stmts{s}, symbols{new SymbolTable(nullptr)} {
  s->parent = this;
}

void BlockStmt::accept(class Visitor &v) {
  return v.visit(this);
};

VarDecl::VarDecl(Identifier* n, Type* t, Expr *e) : name{n}, type{t}, value{e} {
  n->parent = this;
  t->parent = this;
  e->parent = this;
}

void VarDecl::accept(class Visitor &v) {
  return v.visit(this);
};

FuncDecl::FuncDecl(Identifier* n, StmtList* p, Type* r, BlockStmt* s)
: name{n}, params{p}, retType{r}, stmt{s}, symbols{new SymbolTable(nullptr)}{
  n->parent = this;
  p->parent = this;
  r->parent = this;
  s->parent = this;
}

void FuncDecl::accept(class Visitor &v) {
  return v.visit(this);
};

FunctionCall::FunctionCall(Identifier* i, ExprList* l) : name{i}, arguments{l} {
  i->parent = this;
  l->parent = this;
}

void FunctionCall::accept(class Visitor &v) {
  return v.visit(this);
};

IfStmt::IfStmt(Expr* c, BlockStmt* s): cond{c}, stmt{s} {
  c->parent = this;
  s->parent = this;
}

void IfStmt::accept(class Visitor &v) {
  return v.visit(this);
};

WhileStmt::WhileStmt(Expr* c, BlockStmt* s) : cond{c}, stmt{s} {
  c->parent = this;
  s->parent = this;
}

void WhileStmt::accept(class Visitor &v) {
  return v.visit(this);
};

ExprStmt::ExprStmt(Expr* e) : expr{e} {
  e->parent = this;
}

void ExprStmt::accept(class Visitor &v) {
  return v.visit(this);
};

ReturnStmt::ReturnStmt(Expr* e) : expr{e} {
  e->parent = this;
}

void ReturnStmt::accept(class Visitor &v) {
  return v.visit(this);
};

Program::Program(StmtList* s) : block{new BlockStmt(s)}, symbols{new SymbolTable(nullptr)}{
  s->parent = this;
}

void Program::accept(class Visitor &v) {
  return v.visit(this);
};
