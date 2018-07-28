#ifndef AST_EXPR_H
#define AST_EXPR_H

#include "AST/ASTVisitor.h"
#include "AST/TreeElement.h"
#include "Basic/Token.h"
#include "Basic/SourceCode.h"

#include <memory>
#include <stack>

class Decl;

class Expr : public TreeElement {
private:
  /*
   * The static type of the expression. This should be created at construction
   * in all cases. A public accessor is available to get the type, and a
   * protected setter is available to set the type at construction time.
   */
  std::shared_ptr<class Type> fType;

  /*
   * The location in text where this expression was parsed from. This is useful
   * for printing debug information. It can be accessed with the public
   * getLocation method.
   */
  SourceRange fRange;
protected:
  /**
   * Expr Subclasses should call this in their constructor. This sets the
   * location in text where the expression was parsed from so that
   * meaningful error messages can be displayed.
   */
  void setSourceRange(const SourceRange& aRange) {
    fRange = aRange;
  };

  /**
   * Expr Subclasses should call this in their constructor. After construction,
   * all expressions should have a type.
   */
  void setType(std::shared_ptr<class Type> aType) {
    fType = aType;
  };

public:
  /**
   * An enumeration allowing for switching over the dynamic runtime type of an
   * Expression. Very useful when working with polymorphic types.
   */
  enum class Kind {
    #define EXPR(SELF, PARENT) SELF,
    #include "AST/Expr.def"
    #undef EXPR
  };



  // Virtual destructor in base class ensures safe leak-free destruction
  virtual ~Expr() = default;

  /**
   * Convenience method for checking the runtime type of an Expression.
   * Returns true if conversion to derived type is possible. Otherwise
   * return false.
   */
   template<typename T> bool is() const {
     return (dynamic_cast<T*>(this) != nullptr);
   }

   /**
    * Convenience method for casting Expr base type to any one of its derived
    * types. Throws a std::logic_error if conversion is not possible.
    */
  template<typename T> T* as() const {
    T* casted_type = dynamic_cast<T*>(this);
    if (casted_type != nullptr) {
      return casted_type;
    } else {
      std::string error_message_prefix{"unable to cast Expr to "};
      throw std::logic_error(error_message_prefix + typeid(T).name());
    }
  }

  /**
   * All derived type should be able to answer whether or not they are a left
   * value (lvalue). A lvalue is any value which can appear on the left hand
   * side of an equal sign. If an expression returns true, it means that it
   * is able to be assigned to. If not, this method returns false;
   */
  virtual bool isLeftValue() const = 0;

  /**
   * This method returns the runtime type of the expression. All derived types
   * should override this method.
   */
  virtual Expr::Kind getKind() const = 0;

  /**
   * Returns the location in input text where this expression was parsed from.
   */
  SourceRange getSourceRange() const {
    return fRange;
  }

  /**
   * Implements double dispatch for the visitor pattern to be implemented.
   * C++ does not check the runtime type of an expression when selecting
   * which function to use, so in order to implement runtime-type specific
   * processing of the tree, double dispatch is used. In order to use a visitor
   * on an expression tree, simply call t->accept(visitor);
   */
  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  /**
   * Returns the type of the expression. This should never return null, as all
   * expression derived classes are required to set their type in their
   * constructor.
   */
  std::shared_ptr<Type> getType() const {
    return fType;
  };

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


class IdentifierExpr: public Expr {
public:
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const;
  Expr::Kind getKind() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  IdentifierExpr(Token t, std::shared_ptr<class Type> type);
};


/**
 * An Expr subclass that represents a unary expression. Composed of an Expr and
 * an OperatorExpr. All members are guarenteed to be non-null.
 *
 * <UnaryExpr> ::= <OperatorExpr> <Expr>
 */
class UnaryExpr: public Expr {
public:
  Token op;
  std::shared_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Expr::Kind getKind() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  UnaryExpr(Token o, std::shared_ptr<Expr> e, std::shared_ptr<class Type> type);
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
  Token op;
  std::shared_ptr<Expr> right;

  Expr::Kind getKind() const;
  bool isLeftValue() const;
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  std::string getOperator() const {
    return op.lexeme;
  }

  BinaryExpr(std::shared_ptr<Expr> l, Token o, std::shared_ptr<Expr> r, std::shared_ptr<class Type> type);
};

class FunctionCall: public Expr {
public:
  std::shared_ptr<IdentifierExpr> name;
  std::vector<std::shared_ptr<Expr>> arguments;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }
  std::vector<std::shared_ptr<TreeElement>> getChildren() const {
    std::vector<std::shared_ptr<TreeElement>> children;
    children.push_back(name);
    for (auto arg: arguments) {
      children.push_back(arg);
    }
    return children;
  }
  FunctionCall(std::shared_ptr<IdentifierExpr> n, std::vector<std::shared_ptr<Expr>>&& a);
  bool isLeftValue() const;
  Expr::Kind getKind() const;
};

class ListExpr: public Expr {
public:
  std::vector<std::shared_ptr<Expr>> elements;

  Expr::Kind getKind() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  const std::vector<std::shared_ptr<Expr>>& getElements() const {
    return elements;
  }

  ListExpr(std::vector<std::shared_ptr<Expr>>&& elements);
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
  std::vector<std::shared_ptr<Expr>> elements;

  /* Returns a vector of children for easy traversal */
  Expr::Kind getKind() const;
  int size() const;
  bool isLeftValue() const;

  virtual void accept(ASTVisitor& t) const {
     t.visit(*this);
  }

  std::shared_ptr<Expr> operator[] (int x);
  TupleExpr(std::vector<std::shared_ptr<Expr>>&& elements);
};


#endif
