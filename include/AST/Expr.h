#ifndef AST_EXPR_H
#define AST_EXPR_H

#include "AST/Matchable.h"
#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/DeclarationContext.h"
#include "AST/AmbiguousType.h"

#include "Parse/Operator.h"

#include <memory>
#include <stack>

class StackReference;

class Expr : virtual public Matchable {
public:
  enum class Kind {
    #define EXPR(SELF, PARENT) SELF,
    #include "AST/Expr.def"
    #undef EXPR
  };

  AmbiguousType type = {};

  template<typename T> T* as();
  AmbiguousType getType() const;
  virtual Expr::Kind getKind() const = 0;
};

class ExprList : public NonTerminal {
public:
  /* member variables */
  shared_ptr<Expr> element;
  shared_ptr<ExprList> list;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  std::vector<std::shared_ptr<Expr>> vector() const;
  std::shared_ptr<ExprList> reverse() const;
  int size() const;
  std::shared_ptr<Expr> operator[] (int x);
  std::shared_ptr<TypeList> getTypeList(DeclarationContext *c) const;
  template <typename T> bool has();
  ExprList(shared_ptr<Expr> e, shared_ptr<ExprList> l);
  ExprList(std::vector<shared_ptr<Expr>> v);
};


class ExprLabel: public Terminal  {
public:
  Token name;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  ExprLabel(Token n);
};

class LabeledExpr : public Expr, public NonTerminal  {
public:
  shared_ptr<ExprLabel> label;
  shared_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Expr::Kind getKind() const;
  LabeledExpr(shared_ptr<ExprLabel> l, shared_ptr<Expr> e);
};

class StringExpr: public Expr, public Terminal  {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  std::string getString() const;
  StringExpr(std::string s);
  StringExpr(Token t);
};

class IntegerExpr: public Expr, public Terminal  {
public:
  Token token;
  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  int getInt();
  IntegerExpr(int i);
  IntegerExpr(Token t);
};


class BoolExpr: public Expr, public Terminal  {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  bool getBool();
  BoolExpr(bool b);
  BoolExpr(Token t);
};

class DoubleExpr: public Expr, public Terminal  {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  double getDouble();
  DoubleExpr(double i);
  DoubleExpr(Token t);

};

class IdentifierExpr: public Expr, public Terminal  {
public:
  Token token;
  std::shared_ptr<Decl> decl;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  IdentifierExpr(Token t);
};

class TupleExpr: public Expr, public NonTerminal  {
private:

public:
  shared_ptr<ExprList> list;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Expr::Kind getKind() const;
  int size() const;
  static std::shared_ptr<TupleExpr> make(std::vector<std::shared_ptr<Expr>>);
  static std::shared_ptr<TupleExpr> make(std::shared_ptr<ExprList>);

  TupleExpr(shared_ptr<ExprList> l);


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
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  OperatorExpr(Token t);
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
  shared_ptr<class FuncDecl> decl;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Expr::Kind getKind() const;
  UnaryExpr(shared_ptr<OperatorExpr> o, shared_ptr<Expr> e);
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
  shared_ptr<class FuncDecl> decl;

  Expr::Kind getKind() const;
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  BinaryExpr(shared_ptr<Expr> l, shared_ptr<OperatorExpr> o, shared_ptr<Expr> r);
};

class FunctionCall: public Expr, public NonTerminal {
public:
  shared_ptr<IdentifierExpr> name;
  shared_ptr<ExprList> arguments;
  shared_ptr<class FuncDecl> decl;

  FunctionCall(shared_ptr<IdentifierExpr> n, shared_ptr<ExprList> a);
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Expr::Kind getKind() const;
};

class StackReference: public Expr, public Terminal {
public:
  int location;
  StackReference(int l);
  std::string getLexeme() const;
  Expr::Kind getKind() const;
};

ostream& operator<<(ostream& os, Expr* x);
ostream& operator<<(ostream& os, ExprList* x);
ostream& operator<<(ostream& os, ExprLabel* x);

#endif
