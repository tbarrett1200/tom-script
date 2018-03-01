#ifndef AST_EXPR_H
#define AST_EXPR_H

#include "AST/ASTNode.h"
#include "Parse/Token.h"
#include "Parse/Operator.h"
#include "AST/List.h"

using namespace std;
class Expr {
public:
  static std::vector<int> startTokens;
  virtual ~Expr() = default;
};

using ExprList = List<Expr>;


class ExprLabel {
public:
  Token name;
  ExprLabel(Token n);
};

class LabeledExpr : public Expr {
public:
  unique_ptr<ExprLabel> label;
  unique_ptr<Expr> expr;
  LabeledExpr(unique_ptr<ExprLabel> l, unique_ptr<Expr> e);
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

class IdentifierExpr: public Expr {
public:
  Token token;
  IdentifierExpr(Token);
};

class TupleExpr: public Expr {
public:
  unique_ptr<ExprList> list;
  TupleExpr(unique_ptr<List<Expr>>);
};

/**
 * An Expr subclass that represents a operation. Stores the source code token.
 *
 * <UnaryExpr> ::= <OperatorExpr> <Expr>
 */
class OperatorExpr: public Expr {
public:
  Token token;
  OperatorExpr(Token);
};

/**
 * An Expr subclass that represents a unary expression. Composed of an Expr and
 * an OperatorExpr. All members are guarenteed to be non-null.
 *
 * <UnaryExpr> ::= <OperatorExpr> <Expr>
 */
class UnaryExpr: public Expr {
public:
  unique_ptr<OperatorExpr> op;
  unique_ptr<Expr> expr;
  UnaryExpr(unique_ptr<OperatorExpr>, unique_ptr<Expr>);
};

/**
 * An Expr subclass that represents a binary expression. Composed of a left and
 * right Expr and an OperatorExpr specifying the operation. All members are
 * guarenteed to be non-null.
 *
 * <BinaryExpr> ::= <Expr> <OperatorExpr> <Expr>
 */
class BinaryExpr: public Expr {
public:
  unique_ptr<Expr> left;
  unique_ptr<OperatorExpr> op;
  unique_ptr<Expr> right;

  BinaryExpr(unique_ptr<Expr>, unique_ptr<OperatorExpr>, unique_ptr<Expr>);
};

/**
 * An Expr subclass that represents a function call. Composed of an Identifier
 * and a TupleExpr. All members are guarenteed to be non-null.
 *
 * <FunctionCall> ::= <Identifier> <Arguments>
 *
 */
class FunctionCall: public Expr {
public:
  unique_ptr<IdentifierExpr> name;
  unique_ptr<TupleExpr> arguments;

  FunctionCall(unique_ptr<IdentifierExpr>, unique_ptr<TupleExpr>);
};


#endif
