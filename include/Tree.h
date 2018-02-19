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
  Tree* parent;
  virtual void accept(Visitor &v) = 0;
};

class Expr: public Tree {
public:
  virtual void accept(Visitor &v) = 0;
};

class ExprList: public Tree {
public:
  shared_ptr<Expr> stmt;
  shared_ptr<ExprList> next;
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
  shared_ptr<Identifier> name;
  shared_ptr<ExprList> arguments;
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
  shared_ptr<Expr> left;
  shared_ptr<OperatorNode> op;
  shared_ptr<Expr> right;
  BinaryExpr(Expr*, OperatorNode*, Expr*);
  void accept(Visitor &v);
};

struct UnaryExpr: Expr {
public:
  shared_ptr<OperatorNode> op;
  shared_ptr<Expr> expr;
  UnaryExpr(Expr*, OperatorNode*);
  void accept(Visitor &v);
};

class Stmt: public Tree {
public:
  virtual void accept(Visitor &v) = 0;
};

class StmtList: public Tree {
public:
  shared_ptr<Stmt> stmt;
  shared_ptr<StmtList> next;
  StmtList(Stmt*,StmtList*);
  void accept(Visitor &v);
};

class BlockStmt: public Stmt {
public:
  shared_ptr<SymbolTable> symbols;
  shared_ptr<StmtList> stmts;
  BlockStmt(StmtList*);
  void accept(Visitor &v);
};

class VarDecl: public Stmt {
public:
  shared_ptr<Identifier> name;
  shared_ptr<Type> type;
  shared_ptr<Expr> value;
  VarDecl(Identifier*, Type*, Expr*);
  void accept(Visitor &v);
};

class FuncDecl: public Stmt {
public:
  shared_ptr<SymbolTable> symbols;
  shared_ptr<Identifier> name;
  shared_ptr<StmtList> params;
  shared_ptr<Type> retType;
  shared_ptr<BlockStmt> stmt;
  FuncDecl(Identifier*, StmtList*, Type*, BlockStmt*);
  void accept(Visitor &v);
};

class IfStmt: public Stmt {
public:
  shared_ptr<Expr> cond;
  shared_ptr<BlockStmt> stmt;
  IfStmt(Expr*,BlockStmt*);
  void accept(Visitor &v);
};

class WhileStmt: public Stmt {
public:
  shared_ptr<Expr> cond;
  shared_ptr<BlockStmt> stmt;
  WhileStmt(Expr*,BlockStmt*);
  void accept(Visitor &v);
};

class ExprStmt: public Stmt {
public:
  shared_ptr<Expr> expr;
  ExprStmt(Expr*);
  void accept(Visitor &v);
};

class ReturnStmt: public Stmt {
public:
  shared_ptr<Expr> expr;
  ReturnStmt(Expr*);
  void accept(Visitor &v);
};

class Program: public Stmt {
public:
  shared_ptr<SymbolTable> symbols;
  shared_ptr<BlockStmt> block;
  Program(StmtList*);
  void accept(Visitor &v);
};

#endif
