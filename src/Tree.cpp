#include "Visitor.h"
#include "Tree.h"

#include <iostream>

void Tree::defineSymbolTable() {
  if (parent != nullptr) {
    symbols = parent->symbols;
  }
  for (auto child: children) {
    if (child != nullptr) {
      child->defineSymbolTable();
    }
  }
}

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
  children = {left, op, right};
  l->parent = this;
  o->parent = this;
  r->parent = this;
}

void BinaryExpr::accept(class Visitor &v) {
  return v.visit(this);
};

UnaryExpr::UnaryExpr(Expr* e, OperatorNode* o) : op{o}, expr{e} {
  children = {op, expr};
  e->parent = this;
  o->parent = this;
}

void UnaryExpr::accept(class Visitor &v) {
  return v.visit(this);
};

StmtList::StmtList(Stmt* s, StmtList* n) : stmt{s}, next{n} {
  children = {stmt, next};
  s->parent = this;
  if (n != nullptr) {
    n->parent = this;
  }
}

void StmtList::accept(class Visitor &v) {
  return v.visit(this);
}

ExprList::ExprList(Expr* s, ExprList* n) : stmt{s}, next{n} {
  children = {stmt, next};
  s->parent = this;
  if (n != nullptr) n->parent = this;
}

void ExprList::accept(class Visitor &v) {
  return v.visit(this);
}

BlockStmt::BlockStmt(StmtList* s): stmts{s} {
  children = {stmts};
  if (s != nullptr) s->parent = this;
}

void BlockStmt::accept(class Visitor &v) {
  return v.visit(this);
};

void BlockStmt::defineSymbolTable() {
  symbols = shared_ptr<SymbolTable>(new SymbolTable());
  symbols->defineParent(parent->symbols);
  for (auto child: children) {
    if (child != nullptr) {
      child->defineSymbolTable();
    }
  }
}

VarDecl::VarDecl(Identifier* n, Type* t, Expr *e) : name{n}, type{t}, value{e} {
  children = {name, type, value};
  n->parent = this;
  if (t) t->parent = this;
  if (e) e->parent = this;
}

void VarDecl::accept(class Visitor &v) {
  return v.visit(this);
};

FuncDecl::FuncDecl(Identifier* n, StmtList* p, Type* r, BlockStmt* s)
: name{n}, params{p}, retType{r}, stmt{s} {
  children = {name, params, retType, stmt};
  n->parent = this;
  p->parent = this;
  r->parent = this;
  s->parent = this;
}

void FuncDecl::defineSymbolTable() {
  symbols = shared_ptr<SymbolTable>(new SymbolTable());
  symbols->defineParent(parent->symbols);
  for (auto child: children) {
    if (child != nullptr) {
      child->defineSymbolTable();
    }
  }
}

void FuncDecl::accept(class Visitor &v) {
  return v.visit(this);
};

FunctionCall::FunctionCall(Identifier* i, ExprList* l) : name{i}, arguments{l} {
  children = {name, arguments};
  i->parent = this;
  if (l != nullptr) l->parent = this;
}

void FunctionCall::accept(class Visitor &v) {
  return v.visit(this);
};

IfStmt::IfStmt(Expr* c, BlockStmt* s): cond{c}, stmt{s} {
  children = {cond, stmt};
  c->parent = this;
  s->parent = this;
}

void IfStmt::accept(class Visitor &v) {
  return v.visit(this);
};

WhileStmt::WhileStmt(Expr* c, BlockStmt* s) : cond{c}, stmt{s} {
  children = {cond, stmt};
  c->parent = this;
  s->parent = this;
}

void WhileStmt::accept(class Visitor &v) {
  return v.visit(this);
};

ExprStmt::ExprStmt(Expr* e) : expr{e} {
  children = {expr};
  e->parent = this;
}

void ExprStmt::accept(class Visitor &v) {
  return v.visit(this);
};

ReturnStmt::ReturnStmt(Expr* e) : expr{e} {
  children = {expr};
  if (e != nullptr) {
    e->parent = this;
  }
}

void ReturnStmt::accept(class Visitor &v) {
  return v.visit(this);
};

Program::Program(StmtList* s) : block{new BlockStmt(s)} {
  parent = nullptr;
  children = {block};
  if (block != nullptr) block->parent = this;
}

void Program::defineSymbolTable() {
  symbols = shared_ptr<SymbolTable>(new SymbolTable());
  symbols->registerType("Int");
  symbols->registerType("Double");
  symbols->registerType("String");
  symbols->registerType("Function");
  symbols->registerType("Void");
  for (auto child: children) {
    if (child != nullptr) {
      child->defineSymbolTable();
    }
  }
}

void Program::accept(class Visitor &v) {
  return v.visit(this);
};
