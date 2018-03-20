#ifndef AST_EXPR_H
#define AST_EXPR_H

#include "AST/Matchable.h"
#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/DeclarationContext.h"
#include "AST/AmbiguousType.h"

#include "Parse/Operator.h"

#include <memory>

class Expr : virtual public Matchable {
public:
  enum class Kind {
    #define EXPR(SELF, PARENT) SELF,
    #include "AST/Expr.def"
    #undef EXPR
  };

  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }

  virtual Expr::Kind getKind() const = 0;
  virtual AmbiguousType getType(DeclarationContext*) const = 0;
};

class ExprList : public NonTerminal {
public:
  /* member variables */
  shared_ptr<Expr> element;
  shared_ptr<ExprList> list;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    if (!list) return {element};
    else {
      auto children = list->getChildren();
      children.insert(children.begin(), element);
      return children;
    }
  }

  int size() const {
    if (!list) return 1;
    else return list->size()+1;
  }

  std::shared_ptr<Expr> operator[] (int x) {
    if (x == 0) return element;
    else if (!list || x < 0) throw std::logic_error("out of bounds ExprList[]");
    else return (*list)[x-1];
  }
  std::shared_ptr<TypeList> getTypeList(DeclarationContext *c) const;

  template <typename T> bool has() {
    if (list == nullptr) return true;
    else if (!dynamic_cast<T*>(element.get())) return false;
    else return list->has<T>();
  };

  /* Constructor */
  ExprList(shared_ptr<Expr> e, shared_ptr<ExprList> l)
    : element{move(e)}, list{move(l)} {
  }

  ExprList(std::vector<shared_ptr<Expr>> v) {
    element = v.front();
    v.erase(v.begin());
    list = std::make_shared<ExprList>(v);
  }
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
  shared_ptr<ExprLabel> label;
  shared_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {label, expr};
  }

  Expr::Kind getKind() const { return Kind::LabeledExpr; }

  AmbiguousType getType(DeclarationContext*) const;

  LabeledExpr(shared_ptr<ExprLabel> l, shared_ptr<Expr> e): label{move(l)}, expr{move(e)} {
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

  Expr::Kind getKind() const { return Kind::StringExpr; }

  AmbiguousType getType(DeclarationContext*) const;

  std::string getString() const { return token.lexeme.substr(1,token.lexeme.size()-1); }

  StringExpr(std::string s) : token{"\"" + s + "\"", Token::string_literal, 0, 0, 0} {}

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

  Expr::Kind getKind() const { return Kind::IntegerExpr; }

  int getInt() {
    return std::stoi(token.lexeme);
  }

  AmbiguousType getType(DeclarationContext*) const;


  IntegerExpr(int i) : token{to_string(i), Token::integer_literal, 0, 0, 0} {}

  IntegerExpr(Token t) : token{t} {
    if (t.isNot(Token::integer_literal)) {
      throw std::domain_error("IntegerExpr requires a token of type integer_literal");
    }
  }
};


class BoolExpr: public Expr, public Terminal  {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const {
    return token.lexeme;
  }

  Expr::Kind getKind() const { return Kind::BoolExpr; }

  bool getBool() {
    return token.lexeme == "true";
  }

  AmbiguousType getType(DeclarationContext*) const;


  BoolExpr(bool b) {
  if (b) {
     token = Token{"true", Token::kw_true, 0, 0, 0};
   } else {
     token = Token{"false", Token::kw_false, 0, 0, 0};
   }
  }

  BoolExpr(Token t) : token{t} {
    if (!(t.isAny({Token::kw_true, Token::kw_false}))) {
      throw std::domain_error("BoolExpr requires a boolean literal");
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

  Expr::Kind getKind() const { return Kind::DoubleExpr; }

  double getDouble() {
    return std::stod(token.lexeme);
  }

  AmbiguousType getType(DeclarationContext*) const;


  DoubleExpr(int i) : token{to_string(i), Token::double_literal, 0, 0, 0} {}

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

  Expr::Kind getKind() const { return Kind::IdentifierExpr; }
  AmbiguousType getType(DeclarationContext*) const;


  IdentifierExpr(Token t) : token{t} {}
};

class TupleExpr: public Expr, public NonTerminal  {
private:

public:
  shared_ptr<ExprList> list;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {list};
  }

  Expr::Kind getKind() const { return Kind::TupleExpr; }

  int size() const { return list->size(); }
  std::shared_ptr<Expr> operator[] (int x) {
    return (*list)[x];
  }

  AmbiguousType getType(DeclarationContext*) const;


  static std::shared_ptr<TupleExpr> make(std::vector<std::shared_ptr<Expr>>);
  static std::shared_ptr<TupleExpr> make(std::shared_ptr<ExprList>);

  TupleExpr(shared_ptr<ExprList> l) : list{move(l)} {}


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

  Expr::Kind getKind() const { return Kind::OperatorExpr; }
  AmbiguousType getType(DeclarationContext*) const;


  OperatorExpr(Token t) : token{t} {
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
  shared_ptr<OperatorExpr> op;
  shared_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {op, expr};
  }

  Expr::Kind getKind() const { return Kind::UnaryExpr; }

  AmbiguousType getType(DeclarationContext*) const;


  UnaryExpr(shared_ptr<OperatorExpr> o, shared_ptr<Expr> e) : op{move(o)}, expr{move(e)} {
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
  shared_ptr<Expr> left;
  shared_ptr<OperatorExpr> op;
  shared_ptr<Expr> right;

  Expr::Kind getKind() const { return Kind::BinaryExpr; }

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {left, op, right};
  }

  AmbiguousType getType(DeclarationContext*) const;


  BinaryExpr(shared_ptr<Expr> l, shared_ptr<OperatorExpr> o, shared_ptr<Expr> r)
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

class FunctionCall: public Expr, public NonTerminal {
public:
  shared_ptr<IdentifierExpr> name;
  shared_ptr<ExprList> arguments;

  FunctionCall(shared_ptr<IdentifierExpr> n, shared_ptr<ExprList> a) : name{n}, arguments{a} {}

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {name, arguments};
  }

  Expr::Kind getKind() const { return Kind::FunctionCall; }

  AmbiguousType getType(DeclarationContext*) const;

};


ostream& operator<<(ostream& os, Expr* x);
ostream& operator<<(ostream& os, ExprList* x);
ostream& operator<<(ostream& os, ExprLabel* x);

#endif
