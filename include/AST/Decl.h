#ifndef AST_DECL_H
#define AST_DECL_H

#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Matchable.h"
#include "AST/ContextSearchResult.h"

class DeclContext;

class Decl : virtual public Matchable {

public:
  enum class Kind {
    #define DECL(SELF, PARENT) SELF,
    #include "AST/Decl.def"
    #undef DECL
  };
  virtual Decl::Kind getKind() const = 0;
  virtual std::string getName() const = 0;
  virtual shared_ptr<Type> getType() const = 0;
  virtual DeclContext* getContext() const = 0;
  virtual void setContext(DeclContext*) = 0;
};


class DeclName : public Terminal {
public:
  /* member variables */
  Token token;

  std::string getLexeme() const;
  DeclName(Token n) : token{n} {};
};

class TypeAlias : public Decl, public NonTerminal {
private:
  DeclContext *context;

public:
  shared_ptr<DeclName> name;
  shared_ptr<Type> type;

  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  shared_ptr<Type> getType() const;
  DeclContext* getContext() const { return context;}
  void setContext(DeclContext* c) {context = c;}
  TypeAlias(Token n, shared_ptr<Type> t);
};

class VarDecl : public Decl, public NonTerminal {
private:
  DeclContext *context;

public:
  shared_ptr<DeclName> name;
  shared_ptr<Type> type;
  shared_ptr<Expr> expr;

  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  shared_ptr<Type> getType() const;
  DeclContext* getContext() const;
  void setContext(DeclContext* c);
  VarDecl(Token n, shared_ptr<Type> t, shared_ptr<Expr> e);
};

class LetDecl : public Decl, public NonTerminal {
private:
  DeclContext *context;

public:
  shared_ptr<DeclName> name;
  shared_ptr<Type> type;
  shared_ptr<Expr> expr;

  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  DeclContext* getContext() const;
  void setContext(DeclContext* c);
  shared_ptr<Type> getType() const;
  LetDecl(Token n, shared_ptr<Type> t, shared_ptr<Expr> e);
};

class FuncDecl : public Decl, public NonTerminal {
private:
  DeclContext *context;
public:
  shared_ptr<DeclName> name;
  shared_ptr<FunctionType> type;
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  shared_ptr<Type> getType() const;
  DeclContext* getContext() const;
  void setContext(DeclContext* c);
  FuncDecl(Token n, shared_ptr<FunctionType> t);

};
#endif
