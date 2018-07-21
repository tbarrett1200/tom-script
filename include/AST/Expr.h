#ifndef AST_EXPR_H
#define AST_EXPR_H

#include "AST/ASTVisitor.h"
#include "AST/TreeElement.h"
#include "AST/AmbiguousType.h"
#include "Parse/Operator.h"
#include "Basic/Token.h"
#include "Basic/SourceCode.h"
#include "Sema/TypeAnnotatable.h"

#include <memory>
#include <stack>

class Decl;

class Expr : public TypeAnnotatable, public TreeElement {
private:
  std::shared_ptr<Type> type;

public:
  enum class Kind {
    #define EXPR(SELF, PARENT) SELF,
    #include "AST/Expr.def"
    #undef EXPR
  };
  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }
  virtual bool isLeftValue() const = 0;
  virtual Expr::Kind getKind() const = 0;

  SourceLocation getLocation() const {
    return {0, 0};
  }


  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  /* implements the TypeAnnotatable interface */
  void setType(std::shared_ptr<Type> t) {
    type = t;
  };

  std::shared_ptr<Type> getType() const {
    return type;
  };
  bool isTypeSet() const {
    return type != nullptr;
  };
};

class ExprList : public TreeElement  {

public:
  /* member variables */
   std::shared_ptr<Expr> element;
   std::shared_ptr<ExprList> list;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  std::vector<std::shared_ptr<Expr>> vector() const;
  std::shared_ptr<ExprList> reverse() const;
  int size() const;
  std::shared_ptr<Expr>& operator[] (int x);
  std::shared_ptr<TypeList> getTypeList() const;
  template <typename T> bool has();

  ExprList(std::shared_ptr<Expr> e, std::shared_ptr<ExprList> l);
  ExprList(std::vector<std::shared_ptr<Expr>> v);
};


class ExprLabel  : public TreeElement   {
public:
  Token name;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  ExprLabel(Token n);
};

class LabeledExpr : public Expr {
public:
  std::shared_ptr<ExprLabel> label;
  std::shared_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Expr::Kind getKind() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  LabeledExpr(std::shared_ptr<ExprLabel> l, std::shared_ptr<Expr> e);
};

class StringExpr: public Expr {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  std::string getString() const;
  StringExpr(std::string s);
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  StringExpr(Token t);
};

class IntegerExpr: public Expr  {
public:
  Token token;
  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  int getInt() const;
  IntegerExpr(int i);
  bool isLeftValue() const;

  std::string getText() {
    return token.lexeme;
  }

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  IntegerExpr(Token t);
};


class BoolExpr: public Expr {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  bool getBool();
  BoolExpr(bool b);
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  BoolExpr(Token t);
};

class DoubleExpr: public Expr {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  double getDouble() const;
  DoubleExpr(double i);
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }
  std::string getText() const {
    return token.lexeme;
  }
  DoubleExpr(Token t);

};

class ListExpr: public Expr {
public:
  std::shared_ptr<ExprList> data;

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Expr::Kind getKind() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  std::vector<std::shared_ptr<Expr>> getElements() const {
    return data->vector();
  }

  ListExpr(std::shared_ptr<ExprList> d);
};



class IdentifierExpr: public Expr {
public:
  Token token;
  std::shared_ptr<Decl> decl;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  IdentifierExpr(Token t);
};

class AccessorExpr: public Expr {
public:
  std::shared_ptr<IdentifierExpr> id;
  std::shared_ptr<IntegerExpr> index;

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Expr::Kind getKind() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  AccessorExpr(std::shared_ptr<IdentifierExpr> id, std::shared_ptr<IntegerExpr> index);
};


class TupleExpr: public Expr {
private:

public:
  std::shared_ptr<ExprList> list;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Expr::Kind getKind() const;
  int size() const;
  bool isLeftValue() const;
  std::shared_ptr<Expr> operator[] (int x);
  static std::shared_ptr<TupleExpr> make(std::vector<std::shared_ptr<Expr>>);
  static std::shared_ptr<TupleExpr> make(std::shared_ptr<ExprList>);

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  TupleExpr(std::shared_ptr<ExprList> l);


};

/**
 * An Expr subclass that represents a operation. Stores the source code token.
 *
 * <UnaryExpr> ::= <OperatorExpr> <Expr>
 */
class OperatorExpr: public Expr {
public:
  Token token;
  std::shared_ptr<class FuncDecl> decl;
  std::shared_ptr<TypeList> paramType;
  PrecedenceGroup group;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  OperatorExpr(Token t, PrecedenceGroup g);

  OperatorExpr(std::string s);


  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  bool isLeftValue() const;
};

/**
 * An Expr subclass that represents a unary expression. Composed of an Expr and
 * an OperatorExpr. All members are guarenteed to be non-null.
 *
 * <UnaryExpr> ::= <OperatorExpr> <Expr>
 */
class UnaryExpr: public Expr {
public:
  std::shared_ptr<OperatorExpr> op;
  std::shared_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Expr::Kind getKind() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  UnaryExpr(std::shared_ptr<OperatorExpr> o, std::shared_ptr<Expr> e);
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
  std::shared_ptr<Expr> left;
  std::shared_ptr<OperatorExpr> op;
  std::shared_ptr<Expr> right;

  Expr::Kind getKind() const;
  bool isLeftValue() const;
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  std::string getOperator() const {
    return op->token.lexeme;
  }

  BinaryExpr(std::shared_ptr<Expr> l, std::shared_ptr<OperatorExpr> o, std::shared_ptr<Expr> r);
};

class FunctionCall: public Expr {
public:
  std::shared_ptr<IdentifierExpr> name;
  std::shared_ptr<ExprList> arguments;
  std::shared_ptr<class FuncDecl> decl;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  FunctionCall(std::shared_ptr<IdentifierExpr> n, std::shared_ptr<ExprList> a);
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  bool isLeftValue() const;
  Expr::Kind getKind() const;
};

class StackPointer: public Expr {
public:
  int location;
  StackPointer(int l);
  std::string getLexeme() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  Expr::Kind getKind() const;
};

std::ostream& operator<<(std::ostream& os, Expr* x);
std::ostream& operator<<(std::ostream& os, ExprList* x);
std::ostream& operator<<(std::ostream& os, ExprLabel* x);

#endif
