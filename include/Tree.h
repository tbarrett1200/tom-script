#ifndef TREE_H
#define TREE_H

#include <utility>
#include <vector>

#include "Token.h"
#include "Symbols.h"
#include "Operator.h"

using namespace std;

class Visitor;

class Tree {
public:
  shared_ptr<SymbolTable> context;
  virtual void accept(Visitor &v) = 0;
};

class Expr: public Tree {
public:
  virtual void accept(Visitor &v) = 0;
};

class ExprList: public Tree {
public:
  unique_ptr<Expr> stmt;
  unique_ptr<ExprList> next;
  ExprList(Expr*,ExprList*);
  void accept(Visitor &v);
};

class StringLiteral: public Expr {
public:
  Token token;
  StringLiteral(Token);
  void accept(Visitor &v);
};

class IntLiteral: public Expr {
public:
  Token token;
  IntLiteral(Token);
  void accept(Visitor &v);
};

class DoubleLiteral: public Expr {
public:
  Token token;
  DoubleLiteral(Token);
  void accept(Visitor &v);
};

class Identifier: public Expr {
public:
  Token token;
  Identifier(Token);
  void accept(Visitor &v);
};

class FunctionCall: public Expr {
public:
  unique_ptr<Identifier> name;
  unique_ptr<ExprList> arguments;
  FunctionCall(Identifier*, ExprList*);
  void accept(Visitor &v);
};

class Type: public Expr {
public:
  Token token;
  Type(Token);
  void accept(Visitor &v);
};

class OperatorNode: public Tree {
public:
  Token token;
  Operator op;
  OperatorNode(Token);
  void accept(Visitor &v);
};

class BinaryExpr: public Expr {
public:
  unique_ptr<Expr> left;
  unique_ptr<OperatorNode> op;
  unique_ptr<Expr> right;
  BinaryExpr(Expr*, OperatorNode*, Expr*);
  void accept(Visitor &v);
};

struct UnaryExpr: Expr {
public:
  unique_ptr<OperatorNode> op;
  unique_ptr<Expr> expr;
  UnaryExpr(Expr*, OperatorNode*);
  void accept(Visitor &v);
};

class Stmt: public Tree {
public:
  virtual void accept(Visitor &v) = 0;
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
  unique_ptr<Type> type;
  unique_ptr<Expr> value;
  VarDecl(Identifier*, Type*, Expr*);
  void accept(Visitor &v);
};

class FuncDecl: public Stmt {
public:
  unique_ptr<Identifier> name;
  unique_ptr<StmtList> params;
  unique_ptr<Type> retType;
  unique_ptr<BlockStmt> stmt;
  FuncDecl(Identifier*, StmtList*, Type*, BlockStmt*);
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

class Program: public Stmt {
public:
  unique_ptr<BlockStmt> block;
  Program(StmtList*);
  void accept(Visitor &v);
};

#endif
