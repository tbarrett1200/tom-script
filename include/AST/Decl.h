#ifndef AST_DECL_H
#define AST_DECL_H

#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/Matchable.h"

class Decl : virtual public Matchable {
public:
  enum class Kind {
    #define DECL(SELF, PARENT) SELF,
    #include "AST/Decl.def"
    #undef DECL
  };
  virtual Decl::Kind getKind() const = 0;
  virtual std::string getName() const = 0;
};

class DeclName : public Terminal {
public:
  /* member variables */
  Token token;

  /* Returns a vector of children for easy traversal */
  std::string getLexeme() const {
    return token.lexeme;
  }

  /* Constructor */
  DeclName(Token n)
    : token{n} {}
};

class TypeAlias : public Decl, public NonTerminal {
public:
  unique_ptr<DeclName> name;
  unique_ptr<Type> type;

  /* Returns a vector of children for easy traversal */
  std::vector<Matchable*> getChildren() const {
    return {name.get(), type.get()};
  }

  Decl::Kind getKind() const { return Kind::TypeAlias; }
  std::string getName() const { return name->getLexeme(); }

  TypeAlias(Token n, unique_ptr<Type> t)
  : name{new DeclName{n}}, type{move(t)} {}
};

class VarDecl : public Decl, public NonTerminal {
public:
  unique_ptr<DeclName> name;
  unique_ptr<Type> type;
  unique_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<Matchable*> getChildren() const {
    return {name.get(), type.get(), expr.get()};
  }

  Decl::Kind getKind() const { return Kind::VarDecl; }
  std::string getName() const { return name->getLexeme(); }

  VarDecl(Token n, unique_ptr<Type> t, unique_ptr<Expr> e)
  : name{new DeclName{n}}, type{move(t)}, expr{move(e)} {}


};

class LetDecl : public Decl, public NonTerminal {
public:
  unique_ptr<DeclName> name;
  unique_ptr<Type> type;
  unique_ptr<Expr> expr;

  /* Returns a vector of children for easy traversal */
  std::vector<Matchable*> getChildren() const {
    return {name.get(), type.get(), expr.get()};
  }

  Decl::Kind getKind() const { return Kind::LetDecl; }
  std::string getName() const { return name->getLexeme(); }

  LetDecl(Token n, unique_ptr<Type> t, unique_ptr<Expr> e)
  : name{new DeclName{n}}, type{move(t)}, expr{move(e)} {
    if (!expr) {
      throw domain_error("let decl must specify type");
    }
  }

};

class FuncDecl : public Decl, public NonTerminal {
public:
  unique_ptr<DeclName> name;
  unique_ptr<FunctionType> type;

  /* Returns a vector of children for easy traversal */
  std::vector<Matchable*> getChildren() const {
    return {name.get(), type.get()};
  }

  Decl::Kind getKind() const { return Kind::FuncDecl; }
  std::string getName() const { return name->getLexeme(); }

  FuncDecl(Token n, unique_ptr<FunctionType> t)
  : name{new DeclName{n}}, type{move(t)} {
    if (!type) {
      throw domain_error("func decl must specify type");
    }
  }

};
#endif
