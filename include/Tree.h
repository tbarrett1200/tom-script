#ifndef TREE_H
#define TREE_H

#include "Token.h"

using namespace std;

class Visitor;

class Tree {
public:
  virtual void accept(Visitor &v) = 0;
};

class Expr: public Tree {
public:
  virtual void accept(Visitor &v) = 0;
};

class Stmt: public Tree {
public:
  virtual void accept(Visitor &v) = 0;
};

class IntegerLiteral: public Expr {
public:
  string lexeme;
  IntegerLiteral(string);
  void accept(Visitor &v);
};

class Identifier: public Expr {
public:
  string lexeme;
  Identifier(string);
  void accept(Visitor &v);
};

class Operator: public Tree {
public:
  string lexeme;
  Operator(string);
  void accept(Visitor &v);
};

class BinaryExpr: public Expr {
public:
  unique_ptr<Expr> left;
  unique_ptr<Operator> op;
  unique_ptr<Expr> right;
  BinaryExpr(Expr*, Operator*, Expr*);
  void accept(Visitor &v);
};

class StmtList: public Tree {
public:
  unique_ptr<Stmt> stmt;
  unique_ptr<StmtList> next;
  StmtList(Stmt*,StmtList*);
  void accept(Visitor &v);
};

class BlockStmt: public Stmt {
public:
  unique_ptr<StmtList> stmts;
  BlockStmt(StmtList*);
  void accept(Visitor &v);
};

class VarDecl: public Stmt {
public:
  unique_ptr<Identifier> name;
  unique_ptr<Identifier> type;
  VarDecl(Identifier*, Identifier*);
  void accept(Visitor &v);
};

class FuncDecl: public Stmt {
public:
  unique_ptr<Identifier> name;
  unique_ptr<StmtList> params;
  unique_ptr<Identifier> retType;
  unique_ptr<BlockStmt> stmt;
  FuncDecl(Identifier*, StmtList*, Identifier*, BlockStmt*);
  void accept(Visitor &v);
};

class IfStmt: public Stmt {
public:
  unique_ptr<Expr> cond;
  unique_ptr<BlockStmt> stmt;
  IfStmt(Expr*,BlockStmt*);
  void accept(Visitor &v);
};

class WhileStmt: public Stmt {
public:
  unique_ptr<Expr> cond;
  unique_ptr<BlockStmt> stmt;
  WhileStmt(Expr*,BlockStmt*);
  void accept(Visitor &v);
};

class ExprStmt: public Stmt {
public:
  unique_ptr<Expr> expr;
  ExprStmt(Expr*);
  void accept(Visitor &v);
};

class ReturnStmt: public Stmt {
public:
  unique_ptr<Expr> expr;
  ReturnStmt(Expr*);
  void accept(Visitor &v);
};

#endif
