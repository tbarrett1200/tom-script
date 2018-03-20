#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/Matchable.h"
#include "AST/Decl.h"

#include <sstream>

std::string DeclName::getLexeme() const {
  return token.lexeme;
}

std::shared_ptr<Decl> Decl::make(std::shared_ptr<Decl> d, std::function<std::shared_ptr<Expr>(std::shared_ptr<TupleExpr>)> f) {
  d->func = f;
  return d;
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

    auto expr_type = expr->getType(getContext());

    if (!type && expr_type.isAmbiguous()) {
      std::stringstream ss;
      ss << "error: ambiguously typed declaration" << std::endl << expr_type;
      throw ss.str();
    } else if (!type && expr_type.isEmpty()) {
      throw std::string("error: undefined type\n");
    } else if (!type && expr_type.isSingleton()) {
      return expr_type.get();
    }

    auto filtered_type = expr_type.filter([this](std::shared_ptr<Type> t) {
      return *this->type == *t;
    });

    if (filtered_type.isAmbiguous()) {
      std::stringstream ss;
      ss << "error: ambiguously typed declaration" << std::endl << filtered_type;
      throw ss.str();
    } else if (filtered_type.isSingleton()) {
      return filtered_type.get();
    } else {
      throw std::string("error: undefined type\n");
    }
};

DeclarationContext* VarDecl::getContext() const { return context; }
void VarDecl::setContext(DeclarationContext* c) { context = c; }
void VarDecl::setExpr(std::shared_ptr<Expr> e) {
  expr = e;
}

VarDecl::VarDecl(Token n, shared_ptr<Type> t, shared_ptr<Expr> e)
: name{new DeclName{n}}, type{move(t)}, expr{move(e)} {}


std::vector<std::shared_ptr<Matchable>> LetDecl::getChildren() const {
  return {name, type, expr};
}

Decl::Kind LetDecl::getKind() const { return Kind::LetDecl; }
std::string LetDecl::getName() const { return name->getLexeme(); }

void LetDecl::setExpr(std::shared_ptr<Expr> e) {
  expr = e;
}

DeclarationContext* LetDecl::getContext() const { return context; }
void LetDecl::setContext(DeclarationContext* c) { context = c; }

shared_ptr<Type> LetDecl::getType() const {

  auto decl_type = expr->getType(getContext()).filter([this](std::shared_ptr<Type> t){
    return this->type ? this->type == t : true;
  });

  if (decl_type.isAmbiguous()) {
    throw std::string("error: ambiguous type\n");
  } else if (decl_type.isEmpty()) {
    throw std::string("error: types no not match\n");
  } else {
    return decl_type.get();
  }
};

LetDecl::LetDecl(Token n, shared_ptr<Type> t, shared_ptr<Expr> e)
: name{new DeclName{n}}, type{move(t)}, expr{move(e)} {
  if (!expr) {
    throw domain_error("let decl must specify type");
  }
}

std::vector<std::shared_ptr<Matchable>> FuncDecl::getChildren() const {
  if (!stmt) return {name, type};
  else return {name, type, stmt};
}

Decl::Kind FuncDecl::getKind() const { return Kind::FuncDecl; }
std::string FuncDecl::getName() const { return name->getLexeme(); }

shared_ptr<Type> FuncDecl::getType() const {
  return type;
};


DeclarationContext* FuncDecl::getContext() const { return context; }
void FuncDecl::setContext(DeclarationContext* c) { context = c; }

FuncDecl::FuncDecl(Token n, shared_ptr<FunctionType> t, shared_ptr<Stmt> s)
: name{new DeclName{n}}, type{move(t)}, stmt{s} {
  if (!type) {
    throw domain_error("func decl must specify type");
  }
}
