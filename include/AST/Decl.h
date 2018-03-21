#ifndef AST_DECL_H
#define AST_DECL_H

#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Matchable.h"
#include "AST/DeclarationContext.h"

class Stmt;

class Decl : virtual public Matchable {

public:
  enum class Kind {
    #define DECL(SELF, PARENT) SELF,
    #include "AST/Decl.def"
    #undef DECL
  };
  virtual Decl::Kind getKind() const = 0;
  virtual std::string getName() const = 0;

  std::function<std::shared_ptr<Expr>(std::shared_ptr<TupleExpr>)> func = nullptr;

  virtual std::shared_ptr<Expr> getExpr() const = 0;
  virtual void setExpr(std::shared_ptr<Expr>) = 0;

  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }

  static std::shared_ptr<Decl> make(std::shared_ptr<Decl>, std::function<std::shared_ptr<Expr>(std::shared_ptr<TupleExpr>)>);

  virtual shared_ptr<Type> getType() const = 0;
  virtual DeclarationContext* getContext() const = 0;
  virtual void setContext(DeclarationContext*) = 0;
};

ostream& operator<<(ostream& os, Decl& x);

class DeclName : public Terminal {
public:
  /* member variables */
  Token token;

  std::string getLexeme() const;
  DeclName(Token n) : token{n} {};
};


class TypeDecl : public Decl, public NonTerminal {
private:
  DeclarationContext *context;

public:
  shared_ptr<DeclName> name;

  virtual std::shared_ptr<Expr> getExpr() const { return nullptr; };
  virtual void setExpr(std::shared_ptr<Expr> e) {};

  std::vector<std::shared_ptr<Matchable>> getChildren() const { return {name}; }
  Decl::Kind getKind() const {return Kind::TypeDecl; }
  std::string getName() const { return name->token.lexeme; }
  shared_ptr<Type> getType() const { return nullptr; }
  DeclarationContext* getContext() const { return context;}
  void setContext(DeclarationContext* c) {context = c;}
  TypeDecl(Token n) : name{make_shared<DeclName>(n)} {}
};

class TypeAlias : public Decl, public NonTerminal {
private:
  DeclarationContext *context;

public:
  shared_ptr<DeclName> name;
  shared_ptr<Type> type;

  virtual std::shared_ptr<Expr> getExpr() const { return nullptr; };
  virtual void setExpr(std::shared_ptr<Expr> e) {};

  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  shared_ptr<Type> getType() const;
  DeclarationContext* getContext() const { return context;}
  void setContext(DeclarationContext* c) {context = c;}
  TypeAlias(Token n, shared_ptr<Type> t);
};

class VarDecl : public Decl, public NonTerminal {
private:
  DeclarationContext *context;

public:
  shared_ptr<DeclName> name;
  shared_ptr<Type> type;
  std::shared_ptr<Expr> expr;

  std::shared_ptr<Expr> getExpr() const { return expr; }
  void setExpr(std::shared_ptr<Expr>);

  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  shared_ptr<Type> getType() const;
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);
  VarDecl(Token n, shared_ptr<Type> t, shared_ptr<Expr> e);
};

class LetDecl : public Decl, public NonTerminal {
private:
  DeclarationContext *context;

public:
  shared_ptr<DeclName> name;
  shared_ptr<Type> type;
  shared_ptr<Expr> expr;

  std::shared_ptr<Expr> getExpr() const { return expr; }
  void setExpr(std::shared_ptr<Expr>);

  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);
  shared_ptr<Type> getType() const;
  LetDecl(Token n, shared_ptr<Type> t, shared_ptr<Expr> e);
};

/// A named, explicitly typed function
class FuncDecl : public Decl, public NonTerminal {
private:
  DeclarationContext *context;
public:
  shared_ptr<DeclName> name;
  shared_ptr<FunctionType> type;
  shared_ptr<Stmt> stmt;

  /// Constructs a Function Declaration
  FuncDecl(Token n, shared_ptr<FunctionType> t, shared_ptr<Stmt> s);

  /// Declaration Class Overrides
  Decl::Kind getKind() const;
  std::string getName() const;
  shared_ptr<Type> getType() const;

  // Declaration Context Management
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);

  // Matchable Class Overrides
  std::vector<std::shared_ptr<Matchable>> getChildren() const;

  // Interpretation
  std::shared_ptr<Expr> getExpr() const { return nullptr; }
  void setExpr(std::shared_ptr<Expr>) {};

};
#endif
