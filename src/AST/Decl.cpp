#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Decl.h"
#include "AST/Matchable.h"
#include "AST/ContextSearchResult.h"

#include <sstream>

std::string DeclName::getLexeme() const {
  return token.lexeme;
}

std::vector<std::shared_ptr<Matchable>> TypeAlias::getChildren() const {
  return {name, type};
}

Decl::Kind TypeAlias::getKind() const { return Kind::TypeAlias; }
std::string TypeAlias::getName() const { return name->getLexeme(); }
shared_ptr<Type> TypeAlias::getType() const { return type; };

TypeAlias::TypeAlias(Token n, shared_ptr<Type> t)
: name{new DeclName{n}}, type{move(t)} {}


/* Returns a vector of children for easy traversal */
std::vector<std::shared_ptr<Matchable>> VarDecl::getChildren() const {
  return {name, type, expr};
}

Decl::Kind VarDecl::getKind() const { return Kind::VarDecl; }
std::string VarDecl::getName() const { return name->getLexeme(); }
shared_ptr<Type> VarDecl::getType() const {

  if (!expr) return type;

  AmbiguousType exprTypeResult = expr->getType(getContext());
  if (!type) {
    if (exprTypeResult.isAmbiguous()) {
      throw std::string("error: ambiguous type\n");
    } else return exprTypeResult.get();
  } else if (exprTypeResult.isAmbiguous()) {
    if (exprTypeResult.has(type)) {
      return type;
    } else throw std::string("error: types do not match\n");
  } else if (exprTypeResult.has(type)) {
    return type;
  } else throw std::string("error: types do not match\n");
};

DeclContext* VarDecl::getContext() const { return context; }
void VarDecl::setContext(DeclContext* c) { context = c; }

VarDecl::VarDecl(Token n, shared_ptr<Type> t, shared_ptr<Expr> e)
: name{new DeclName{n}}, type{move(t)}, expr{move(e)} {}


std::vector<std::shared_ptr<Matchable>> LetDecl::getChildren() const {
  return {name, type, expr};
}

Decl::Kind LetDecl::getKind() const { return Kind::LetDecl; }
std::string LetDecl::getName() const { return name->getLexeme(); }

DeclContext* LetDecl::getContext() const { return context; }
void LetDecl::setContext(DeclContext* c) { context = c; }

shared_ptr<Type> LetDecl::getType() const {

  AmbiguousType exprTypeResult = expr->getType(getContext());

  if (!type) {
    if (exprTypeResult.isAmbiguous()) {
      throw std::string("error: ambiguous type\n");
    } else return exprTypeResult.get();
  } else if (exprTypeResult.isAmbiguous()) {
    if (exprTypeResult.has(type)) {
      return type;
    } else throw std::string("error: types do not match\n");
  } else if (exprTypeResult.has(type)) {
    return type;
  } else {
    std::stringstream ss;
    ss << "error: types do not match: " << type << " and " << exprTypeResult.get() << std::endl;
    throw ss.str();
  }
};

LetDecl::LetDecl(Token n, shared_ptr<Type> t, shared_ptr<Expr> e)
: name{new DeclName{n}}, type{move(t)}, expr{move(e)} {
  if (!expr) {
    throw domain_error("let decl must specify type");
  }
}

std::vector<std::shared_ptr<Matchable>> FuncDecl::getChildren() const {
  return {name, type};
}

Decl::Kind FuncDecl::getKind() const { return Kind::FuncDecl; }
std::string FuncDecl::getName() const { return name->getLexeme(); }

shared_ptr<Type> FuncDecl::getType() const {
  return type;
};


DeclContext* FuncDecl::getContext() const { return context; }
void FuncDecl::setContext(DeclContext* c) { context = c; }

FuncDecl::FuncDecl(Token n, shared_ptr<FunctionType> t)
: name{new DeclName{n}}, type{move(t)} {
  if (!type) {
    throw domain_error("func decl must specify type");
  }
}
