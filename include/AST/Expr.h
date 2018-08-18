#ifndef AST_EXPR_H
#define AST_EXPR_H

#include <memory>

#include "Basic/Token.h"
#include "Basic/SourceCode.h"

#include "AST/TreeElement.h"


class Decl;

class Expr : public TreeElement {
private:
  /*
   * The static type of the expression. This should be created at construction
   * in all cases. A public accessor is available to get the type, and a
   * protected setter is available to set the type at construction time.
   */
   const class Type *fType;

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

public:
  /**
   * Expr Subclasses should call this in their constructor. After construction,
   * all expressions should have a type.
   */
  void setType(const class Type *aType) {
    fType = aType;
  };

  virtual std::string name() const override {
    return "expr";
  };

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
   * Returns the type of the expression. This should never return null, as all
   * expression derived classes are required to set their type in their
   * constructor.
   */
  const class Type* getType() const {
    return fType;
  };


};


class StringExpr: public Expr {
private:
  Token token_;

public:

  StringRef lexeme() const {
    return token_.lexeme();
  }

  Expr::Kind getKind() const override { return Kind::StringExpr; }

  bool isLeftValue() const override {
    return false;
  }

  std::string getString() const {
    std::string str = token_.lexeme().str();
    return str.substr(1, str.size()-2);
  }


  std::string name() const override {
    return "string-literal-expression";
  };

  StringExpr(Token t) : token_{t} {
    throw std::logic_error("string not implemented");

    if (t.isNot(Token::string_literal)) {
      throw std::domain_error("StringExpr requires a token of type string_literal");
    }
  }



};

class IntegerExpr: public Expr  {
private:
  Token token_;

public:

  IntegerExpr(Token t) : token_{t} {
    if (t.isNot(Token::integer_literal)) {
      throw std::domain_error("IntegerExpr requires a token of type integer_literal");
    }
  }

  StringRef lexeme() const {
    return token_.lexeme();
  }

  std::string name() const override {
    return "integer-literal-expression";
  };

  Expr::Kind getKind() const override { return Kind::IntegerExpr; }

  int getInt() const {
    return std::stoi(token_.lexeme().str());
  }

  bool isLeftValue() const override {
    return false;
  }


};


class BoolExpr: public Expr {
private:
  Token token_;

public:

  /* Returns a vector of children for easy traversal */
  StringRef lexeme() const {
    return token_.lexeme();
  }

  Expr::Kind  getKind() const override { return Kind::BoolExpr; }

  bool getBool() const {
    return token_.lexeme() == "true" ? 1: 0;
  }

  std::string name() const override {
    return "boolean-literal-expression";
  };

  bool isLeftValue() const override {
    return false;
  }

  BoolExpr(Token t) : token_{t} {
    if (!(t.isAny({Token::kw_true, Token::kw_false}))) {
      throw std::domain_error("BoolExpr requires a boolean literal");
    }
  }

};

class DoubleExpr: public Expr {
private:
  Token token_;

public:

  /* Returns a vector of children for easy traversal */
  StringRef lexeme() const {
    return token_.lexeme();
  }

  Expr::Kind getKind() const override { return Kind::DoubleExpr; }

  bool isLeftValue() const override {
    return false;
  }

  std::string name() const override {
    return "double-literal-expression";
  };

  double getDouble() const {
    return std::stod(token_.lexeme().str());
  }

  DoubleExpr(Token t) : token_{t} {
    if (t.isNot(Token::double_literal)) {
      throw std::domain_error("DoubleExpr requires a token of type double_literal");
    }
  }

};


class IdentifierExpr: public Expr {
private:
  Token token_;

public:

  StringRef lexeme() const {
    return token_.lexeme();
  }

  Expr::Kind getKind() const override { return Kind::IdentifierExpr; }

  bool isLeftValue() const override {
    return true;
  }

  IdentifierExpr(Token tok) : token_{tok} {}

  std::string name() const override {
    return "identifier-expression";
  };

};


/**
 * An Expr subclass that represents a unary expression. Composed of an Expr and
 * an OperatorExpr. All members are guarenteed to be non-null.
 *
 * <UnaryExpr> ::= <OperatorExpr> <Expr>
 */
class UnaryExpr: public Expr {
private:
  Token op_;
  std::unique_ptr<Expr> expr_;
public:

  std::vector<TreeElement*> getChildren() const override {
    return {expr_.get()};
  }

  Expr::Kind getKind() const override { return Kind::UnaryExpr; }

  bool isLeftValue() const override {
    return false;
  }

  UnaryExpr(Token o, std::unique_ptr<Expr> e) : op_{std::move(o)}, expr_{std::move(e)} {
    if (!expr_) {
      throw std::domain_error("BinaryExpr: expr is required");
    }
  }

  std::string name() const override {
    return "unary-expression";
  };

  StringRef getOperator() const {
    return op_.lexeme();
  }

  const Expr& getExpr() const {
    return *expr_;
  }

  Expr& getExpr() {
    return *expr_;
  }


};

/**
 * An Expr subclass that represents a binary expression. Composed of a left and
 * right Expr and an OperatorExpr specifying the operation. All members are
 * guarenteed to be non-null.
 *
 * <BinaryExpr> ::= <Expr> <OperatorExpr> <Expr>
 */
class BinaryExpr: public Expr {
private:
  std::unique_ptr<Expr> left_;
  Token op_;
  std::unique_ptr<Expr> right_;
public:

  Expr::Kind getKind() const override { return Kind::BinaryExpr; }

  std::vector<TreeElement*> getChildren() const override {
    return {left_.get(), right_.get()};
  }

  bool isLeftValue() const override {
    return false;
  }

  std::string name() const override {
    return "binary-expression";
  };

  BinaryExpr(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r)
  : left_{std::move(l)}, op_{o}, right_{std::move(r)} {
    if (!left_) {
      throw std::domain_error("BinaryExpr: left is required");
    }

    if (!op_.is(Token::operator_id)) {
      throw std::domain_error("BinaryExpr: invalid operator");
    }

    if (!right_) {
      throw std::domain_error("BinaryExpr: right is required");
    }
  }


  const Expr& getLeft() const {
    return *left_;
  }

  const Expr& getRight() const {
    return *right_;
  }
  Expr& getLeft() {
    return *left_;
  }

  Expr& getRight() {
    return *right_;
  }
  StringRef getOperator() const {
    return op_.lexeme();
  }

};

class FunctionCall: public Expr {
private:
  std::unique_ptr<IdentifierExpr> name_;
  std::vector<std::unique_ptr<Expr>> arguments_;
public:

  FunctionCall(std::unique_ptr<IdentifierExpr> n, std::vector<std::unique_ptr<Expr>> a)
  : name_{std::move(n)}, arguments_{std::move(a)} {
  }

  bool isLeftValue() const override {
    return false;
  }

  Expr::Kind getKind() const override { return Kind::FunctionCall; }

  std::string name() const override {
    return "function-call-expression";
  };

  std::vector<TreeElement*> getChildren() const override {
    std::vector<TreeElement*> children;
    for (auto &arg: arguments_) {
      children.push_back(arg.get());
    }
    return children;
  }
  const std::vector<std::unique_ptr<Expr>>& getArguments() const {
    return arguments_;
  }
  StringRef getFunctionName() const {
    return name_->lexeme();
  }

};

class ListExpr: public Expr {
private:
  std::vector<std::unique_ptr<Expr>> elements_;

public:

  Expr::Kind getKind() const override {
    return Kind::ListExpr;
  }
  bool isLeftValue() const override {
    return false;
  }

  ListExpr(std::vector<std::unique_ptr<Expr>>&& d): elements_{std::move(d)} {
    throw std::logic_error("list not implemented");
  }

  std::string name() const override {
    return "list-expression";
  };



  const std::vector<std::unique_ptr<Expr>>& getElements() const {
    return elements_;
  }

};

class AccessorExpr: public Expr {
private:
  std::unique_ptr<IdentifierExpr> id_;
  std::unique_ptr<IntegerExpr> index_;

public:

  std::vector<TreeElement*> getChildren() const override {
    return {id_.get(), index_.get()};
  }

  Expr::Kind getKind() const override {
    return Expr::Kind::AccessorExpr;
  }

  bool isLeftValue() const override {
    return true;
  }

  std::string name() const override {
    return "accessor-expression";
  };

  AccessorExpr(std::unique_ptr<IdentifierExpr> a, std::unique_ptr<IntegerExpr> b): id_{std::move(a)}, index_{std::move(b)} {
    throw std::logic_error("accessor not implemented");
  }


};

class TupleExpr: public Expr {
private:
  std::vector<std::unique_ptr<Expr>> elements_;

public:

  /* Returns a vector of children for easy traversal */
  Expr::Kind getKind() const override { return Kind::TupleExpr; }

  bool isLeftValue() const override{
    return false;
  }

  int size() const { return elements_.size(); }

  Expr& operator[] (int x) {
    return *elements_[x];
  }

  std::string name() const override {
    return "tuple-expression";
  };

  TupleExpr(std::vector<std::unique_ptr<Expr>> list) : elements_{std::move(list)} {
    throw std::logic_error("tuple not implemented");
  }

};


#endif
