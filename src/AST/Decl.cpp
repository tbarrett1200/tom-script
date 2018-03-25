#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/Matchable.h"
#include "AST/Decl.h"

#include <sstream>

ostream& operator<<(ostream& os, Decl& x) {

  if (x.getKind() == Decl::Kind::TypeAlias) {
    auto t = x.getType();
    auto f = x.getContext()->getFundamentalType(t);
    os << x.getName() << " => " << t;
    if (!(*f == *t)) os << " => " << f;
  } else {
    auto t = x.getType();
    auto f = x.getContext()->getFundamentalType(t);
    os << x.getName() << ": " << t;
    if (!(*f == *t)) os << " => " << f;
  }
  return os;
}

std::string DeclName::getLexeme() const {
  return token.lexeme;
}

std::shared_ptr<Decl> Decl::make(std::shared_ptr<Decl> d, std::function<void(RuntimeStack& vector)> f) {
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

  if (expr && type) {
    auto decl_type = expr->getType().filter([this](std::shared_ptr<Type> t){
        return equal(t, this->type, this->getContext());
    });

    if (decl_type.isAmbiguous()) {
      std::stringstream ss;
      ss << "error: ambiguous type" << std::endl << decl_type;
      throw ss;
    } else if (decl_type.isEmpty()) {
      std::stringstream ss;
      ss << "error: expression type does not resolve to " << std::endl << getContext()->getFundamentalType(type);
      throw ss;
    } else {
      return type;
    }
  } else if (expr) {
    if (expr->type.isAmbiguous()) {
      std::stringstream ss;
      ss << "error: ambiguous type" << std::endl << expr->type;
      throw ss;
    } else if (expr->type.isEmpty()) {
      std::stringstream ss;
      ss << "error: expression type does not resolve to " << std::endl << getContext()->getFundamentalType(type);
      throw ss;
    } else {
      return expr->type.get();
    }
  } else if (type) {
    return type;
  } else return nullptr;
}



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


    if (expr && type) {
      auto decl_type = expr->getType().filter([this](std::shared_ptr<Type> t){
        return equal(t, this->type, this->getContext());
      });
      if (decl_type.isAmbiguous()) {
        std::stringstream ss;
        ss << "error: ambiguous type" << std::endl << decl_type;
        throw ss;
      } else if (decl_type.isEmpty()) {
        std::stringstream ss;
        ss << "error: expression type does not resolve to " << std::endl << getContext()->getFundamentalType(type);
        throw ss;
      } else {
        return type;
      }
    } else if (expr) {
      if (expr->type.isAmbiguous()) {
        std::stringstream ss;
        ss << "error: ambiguous type" << std::endl << expr->type;
        throw ss;
      } else if (expr->type.isEmpty()) {
        std::stringstream ss;
        ss << "error: expression type does not resolve to " << std::endl << getContext()->getFundamentalType(type);
        throw ss;
      } else {
        return expr->type.get();
      }
    } else if (type) {
      return type;
    } else return nullptr;
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

DeclarationContext* FuncDecl::getContext() const { return context.get(); }
void FuncDecl::setContext(DeclarationContext* c) { context->setParent(c); }

FuncDecl::FuncDecl(Token n, shared_ptr<ParamDeclList> p, shared_ptr<Type> r, shared_ptr<CompoundStmt> s)
: name{new DeclName{n}}, params{p}, returnType{r},
  type{new FunctionType(p ? p->getTypeList() : nullptr, r)}, stmt{s} {}
