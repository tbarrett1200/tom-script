#ifndef AST_EXPR_H
#define AST_EXPR_H

#include "AST/ASTNode.h"
#include "AST/Matchable.h"
#include "Parse/Token.h"
#include "Parse/Operator.h"

using namespace std;

class Expr : virtual public Matchable {
public:
};


class ExprList : public NonTerminal {
public:
  /* member variables */
  unique_ptr<Expr> element;
  unique_ptr<ExprList> list;

  /* Returns a vector of children for easy traversal */
  std::vector<Matchable*> getChildren() const {
    if (!list) return {element.get()};
    else {
      auto children = list->getChildren();
      children.insert(children.begin(), element.get());
      return children;
    }
  }

  template <typename T> bool has() {
    if (list == nullptr) return true;
    else if (!dynamic_cast<T*>(element.get())) return false;
    else return list->has<T>();
  };

  /* Constructor */
  ExprList(unique_ptr<Expr> e, unique_ptr<ExprList> l)
    : element{move(e)}, list{move(l)} {}
};


class ExprLabel: public Terminal  {
public:
  Token name;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const {
    return name.lexeme;
  }

  ExprLabel(Token n): name{n} {};
};

class LabeledExpr : public Expr, public NonTerminal  {
public:
  unique_ptr<ExprLabel> label;
  unique_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<Matchable*> getChildren() const {
    return {label.get(), expr.get()};
  }

  LabeledExpr(unique_ptr<ExprLabel> l, unique_ptr<Expr> e): label{move(l)}, expr{move(e)} {
    if (!label) {
      throw std::domain_error("labeled expr: label is required");
    }
    if (!expr) {
      throw std::domain_error("labeled expr: expr is required");
    }
  }
};

class StringExpr: public Expr, public Terminal  {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const {
    return token.lexeme;
  }

  StringExpr(Token t) : token{t} {
    if (t.isNot(Token::string_literal)) {
      throw std::domain_error("StringExpr requires a token of type string_literal");
    }
  }
};

class IntegerExpr: public Expr, public Terminal  {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const {
    return token.lexeme;
  }

  IntegerExpr(Token t) : token{t} {
    if (t.isNot(Token::integer_literal)) {
      throw std::domain_error("IntegerExpr requires a token of type integer_literal");
    }
  }
};

class DoubleExpr: public Expr, public Terminal  {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const {
    return token.lexeme;
  }

  DoubleExpr(Token t) : token{t} {
    if (t.isNot(Token::double_literal)) {
      throw std::domain_error("DoubleExpr requires a token of type double_literal");
    }
  }
};

class IdentifierExpr: public Expr, public Terminal  {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const {
    return token.lexeme;
  }

  IdentifierExpr(Token t) : token{t} {
    if (t.isNot(Token::identifier)) {
      throw std::domain_error("Identifier requires a token of type identifier");
    }
  }
};

class TupleExpr: public Expr, public NonTerminal  {
public:
  unique_ptr<ExprList> list;

  /* Returns a vector of children for easy traversal */
  std::vector<Matchable*> getChildren() const {
    return {list.get()};
  }

  TupleExpr(unique_ptr<ExprList> l) : list{move(l)} {}
};

/**
 * An Expr subclass that represents a operation. Stores the source code token.
 *
 * <UnaryExpr> ::= <OperatorExpr> <Expr>
 */
class OperatorExpr: public Expr, public Terminal {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const {
    return token.lexeme;
  }
  OperatorExpr(Token t) {
    if (t.isNot(Token::operator_id)) {
      throw std::domain_error("OperatorExpr requires a token of type operator_id");
    }
  }
};

/**
 * An Expr subclass that represents a unary expression. Composed of an Expr and
 * an OperatorExpr. All members are guarenteed to be non-null.
 *
 * <UnaryExpr> ::= <OperatorExpr> <Expr>
 */
class UnaryExpr: public Expr, public NonTerminal  {
public:
  unique_ptr<OperatorExpr> op;
  unique_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<Matchable*> getChildren() const {
    return {op.get(), expr.get()};
  }

  UnaryExpr(unique_ptr<OperatorExpr> o, unique_ptr<Expr> e) : op{move(o)}, expr{move(e)} {
    if (!op) {
      throw std::domain_error("BinaryExpr: op is required");
    }
    if (!expr) {
      throw std::domain_error("BinaryExpr: expr is required");
    }
  }
};

/**
 * An Expr subclass that represents a binary expression. Composed of a left and
 * right Expr and an OperatorExpr specifying the operation. All members are
 * guarenteed to be non-null.
 *
 * <BinaryExpr> ::= <Expr> <OperatorExpr> <Expr>
 */
class BinaryExpr: public Expr, public NonTerminal  {
public:
  unique_ptr<Expr> left;
  unique_ptr<OperatorExpr> op;
  unique_ptr<Expr> right;

  BinaryExpr(unique_ptr<Expr> l, unique_ptr<OperatorExpr> o, unique_ptr<Expr> r)
  : left{move(l)}, op{move(o)}, right{move(r)} {
    if (!left) {
      throw std::domain_error("BinaryExpr: left is required");
    }
    if (!op) {
      throw std::domain_error("BinaryExpr: op is required");
    }
    if (!right) {
      throw std::domain_error("BinaryExpr: right is required");
    }
  }
};

/**
 * An Expr subclass that represents a function call. Composed of an Identifier
 * and a TupleExpr. All members are guarenteed to be non-null.
 *
 * <FunctionCall> ::= <Identifier> <Arguments>
 *
 */
class FunctionCall: public Expr, public NonTerminal {
public:
  unique_ptr<IdentifierExpr> name;
  unique_ptr<TupleExpr> arguments;

  /* Returns a vector of children for easy traversal */
  std::vector<Matchable*> getChildren() const {
    return {name.get(), arguments.get()};
  }

  FunctionCall(unique_ptr<IdentifierExpr> n, unique_ptr<TupleExpr> a)
  : name{move(n)}, arguments{move(a)} {
    if (!name) {
      throw std::domain_error("function call: name is required");
    }
  }
};


#endif
