#ifndef TREE_H
#define TREE_H

#include "Token.h"
#include "Symbols.h"

#include <utility>
#include <vector>
#include <functional>
using namespace std;

class Visitor;

class Tree {
public:
  shared_ptr<SymbolTable> context;
  virtual void accept(Visitor &v) = 0;
};

template <class T> class List {
public:
  shared_ptr<T> next;
  List<T> filter(std::function<bool(T,int,List<T>)>);
};

class Expr: public Tree {
public:
  enum Type {
    Int, Double, String, Function
  };
  Expr::Type type;
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
  Expr::Type type = Expr::Type::String;
  Token token;
  StringLiteral(Token);
  void accept(Visitor &v);
};

class IntLiteral: public Expr {
public:
  Expr::Type type = Expr::Type::Int;
  Token token;
  IntLiteral(Token);
  void accept(Visitor &v);
};

class DoubleLiteral: public Expr {
public:
  Expr::Type type = Expr::Type::Double;
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

class Operator: public Tree {
public:
  Token token;
  Operator(Token);
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
