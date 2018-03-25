#ifndef AST_DECL_H
#define AST_DECL_H

#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Matchable.h"
#include "AST/DeclarationContext.h"
#include "Sema/RuntimeStack.h"

#include <vector>

class CompoundStmt;

class Decl : virtual public Matchable {

public:
  enum class Kind {
    #define DECL(SELF, PARENT) SELF,
    #include "AST/Decl.def"
    #undef DECL
  };
  virtual Decl::Kind getKind() const = 0;
  virtual std::string getName() const = 0;

  std::function<void(RuntimeStack&)> func = nullptr;
  std::shared_ptr<StackReference> location;

  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }

  static std::shared_ptr<Decl> make(std::shared_ptr<Decl>, std::function<void(RuntimeStack& stack)>);

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


class ParamDecl : public Decl, public NonTerminal {
private:
  DeclarationContext *context;

public:
  std::shared_ptr<DeclName> primary;
  std::shared_ptr<DeclName> secondary;
  std::shared_ptr<Type> type;
  std::shared_ptr<Expr> default_value;

  ParamDecl(Token p, Token s, shared_ptr<Type> t)
  : primary{std::make_shared<DeclName>(p)},
    secondary{std::make_shared<DeclName>(s)},
    type{t} {}

  Decl::Kind getKind() const { return Decl::Kind::ParamDecl; }
  std::string getName() const { return secondary->getLexeme(); }
  shared_ptr<Type> getType() const {
    if (primary->token.lexeme == "_") return type;
    else return std::make_shared<LabeledType>(std::make_shared<TypeLabel>(primary->token), type);
  }

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return { primary, secondary, type };
  }

  // Declaration Context Management
  DeclarationContext* getContext() const { return context; }
  void setContext(DeclarationContext* c) { context = c; }
};

class ParamDeclList : public NonTerminal {
public:
  std::shared_ptr<ParamDecl> element;
  std::shared_ptr<ParamDeclList> list;

  ParamDeclList(std::shared_ptr<ParamDecl> e, std::shared_ptr<ParamDeclList> l)
  : element{e}, list{l} {
    if (!e) throw logic_error("violated precondition: element is required");
  }

  std::shared_ptr<TypeList> getTypeList() const {
    return std::make_shared<TypeList>(element->getType(), list ? list->getTypeList(): nullptr);
  }

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return { element, list };
  }
  void setContext(DeclarationContext* c) {
    element->setContext(c);
    if (list) list->setContext(c);
  }
};

/// A named, explicitly typed function
class FuncDecl : public Decl, public NonTerminal {
private:
  std::shared_ptr<DeclarationContext> context = make_shared<DeclarationContext>();

public:
  shared_ptr<DeclName> name;
  shared_ptr<ParamDeclList> params;
  shared_ptr<Type> returnType;
  shared_ptr<FunctionType> type;
  shared_ptr<CompoundStmt> stmt;

  /// Constructs a Function Declaration
  FuncDecl(Token n, shared_ptr<ParamDeclList> t, shared_ptr<Type>, shared_ptr<CompoundStmt> s);

  /// Declaration Class Overrides
  Decl::Kind getKind() const;
  std::string getName() const;
  shared_ptr<Type> getType() const;

  // Declaration Context Management
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);

  // Matchable Class Overrides
  std::vector<std::shared_ptr<Matchable>> getChildren() const;

};
#endif
