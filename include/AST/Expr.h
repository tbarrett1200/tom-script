#ifndef AST_EXPR_H
#define AST_EXPR_H

#include "AST/ASTNode.h"
#include "Parse/Token.h"
#include "Parse/Operator.h"

using namespace std;

class Expr {
public:
  static std::vector<int> startTokens;
};


using ExprLabel = Meta<Expr, Expr>;
using LabeledExpr = Meta<Expr, ExprLabel, Expr>;

class ExprList {
public:
  unique_ptr<Expr> element;
  unique_ptr<ExprList> list;
  ExprList(unique_ptr<Expr>, unique_ptr<ExprList>);
};

class StringExpr: public Expr {
public:
  Token token;
  StringExpr(Token);
};

class IntegerExpr: public Expr {
public:
  Token token;
  IntegerExpr(Token);
};

class DoubleExpr: public Expr {
public:
  Token token;
  DoubleExpr(Token);
};

class Identifier: public Expr {
public:
  Token token;
  Identifier(Token);
};

class TupleExpr: public Expr {
public:
  unique_ptr<ExprList> list;
  TupleExpr(unique_ptr<ExprList>);
};

class OperatorExpr: public Expr {
public:
  Token token;
  Operator op;
  OperatorExpr(Token);
};

class UnaryExpr: public Expr {
public:
  unique_ptr<OperatorExpr> op;
  unique_ptr<Expr> expr;
  UnaryExpr(unique_ptr<OperatorExpr>, unique_ptr<Expr>);
};

class BinaryExpr: public Expr {
public:
  unique_ptr<Expr> left;
  unique_ptr<OperatorExpr> op;
  unique_ptr<Expr> right;
  BinaryExpr(unique_ptr<Expr>, unique_ptr<OperatorExpr>, unique_ptr<Expr>);
};

class FunctionCall: public Expr {
public:
  unique_ptr<Identifier> name;
  unique_ptr<TupleExpr> arguments;
  FunctionCall(unique_ptr<Identifier>, unique_ptr<TupleExpr>);
};

#endif
