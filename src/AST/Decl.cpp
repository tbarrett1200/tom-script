#include "Basic/Token.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/TreeElement.h"
#include "AST/Decl.h"

#include <sstream>

//=--------------------------------------------------------------------------=//
// OperatorExpr
//=--------------------------------------------------------------------------=//

DeclName::DeclName(Token n) : token{n} {};

std::string DeclName::getLexeme() const {
  return token.lexeme;
}

//=--------------------------------------------------------------------------=//
// TypeDecl
//=--------------------------------------------------------------------------=//

std::shared_ptr<Expr> TypeDecl::getExpr() const { return nullptr; };
void setExpr(std::shared_ptr<Expr> e) {};

std::vector<std::shared_ptr<TreeElement>> TypeDecl::getChildren() const { return {name}; }
Decl::Kind TypeDecl::getKind() const {return Kind::TypeDecl; }
std::string TypeDecl::getName() const { return name->token.lexeme; }

TypeDecl::TypeDecl(Token n) : name{std::make_shared<DeclName>(n)} {}

//=--------------------------------------------------------------------------=//
// TypeAlias
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<TreeElement>> TypeAlias::getChildren() const {
  return {name};
}
std::shared_ptr<Expr> TypeAlias::getExpr() const { return nullptr; };
void TypeAlias::setExpr(std::shared_ptr<Expr> e) {};
Decl::Kind TypeAlias::getKind() const { return Kind::TypeAlias; }
std::string TypeAlias::getName() const { return name->getLexeme(); }

TypeAlias::TypeAlias(Token n, std::shared_ptr<Type> t)
: name{new DeclName{n}}, type{move(t)} {}

//=--------------------------------------------------------------------------=//
// VarDecl
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::vector<std::shared_ptr<TreeElement>> VarDecl::getChildren() const {
  return {name, expr};
}

std::shared_ptr<Expr> VarDecl::getExpr() const { return expr; }

Decl::Kind VarDecl::getKind() const { return Kind::VarDecl; }
std::string VarDecl::getName() const { return name->getLexeme(); }

VarDecl::VarDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e)
: name{new DeclName{n}}, type{move(t)}, expr{move(e)} {}

//=--------------------------------------------------------------------------=//
// LetDecl
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<TreeElement>> LetDecl::getChildren() const {
  return {name, expr};
}

Decl::Kind LetDecl::getKind() const { return Kind::LetDecl; }
std::string LetDecl::getName() const { return name->getLexeme(); }

void LetDecl::setExpr(std::shared_ptr<Expr> e) {
  expr = e;
}
std::shared_ptr<Expr> LetDecl::getExpr() const { return expr; }

LetDecl::LetDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e)
: name{new DeclName{n}}, type{move(t)}, expr{move(e)} {
  if (!expr) {
    throw std::domain_error("let decl must specify type");
  }
}

//=--------------------------------------------------------------------------=//
// ParamDecl
//=--------------------------------------------------------------------------=//
ParamDecl::ParamDecl(Token n, std::shared_ptr<Type> t)
: name{std::make_shared<DeclName>(n)}, type{t} {}

Decl::Kind ParamDecl::getKind() const { return Decl::Kind::ParamDecl; }
std::string ParamDecl::getName() const { return name->getLexeme(); }

std::vector<std::shared_ptr<TreeElement>> ParamDecl::getChildren() const {
  return { name };
}

//=--------------------------------------------------------------------------=//
// ParamDeclList
//=--------------------------------------------------------------------------=//

  ParamDeclList::ParamDeclList(std::shared_ptr<ParamDecl> e, std::shared_ptr<ParamDeclList> l)
  : element{e}, list{l} {
    if (!e) throw std::logic_error("violated precondition: element is required");
  }


  std::vector<std::shared_ptr<TreeElement>> ParamDeclList::getChildren() const {
    return { element, list };
  }

//=--------------------------------------------------------------------------=//
// FuncDecl
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<TreeElement>> FuncDecl::getChildren() const {
  if (!stmt) return {name};
  else return {name, stmt};
}

Decl::Kind FuncDecl::getKind() const { return Kind::FuncDecl; }
std::string FuncDecl::getName() const { return name->getLexeme(); }


FuncDecl::FuncDecl(Token n, std::shared_ptr<ParamDeclList> p, std::shared_ptr<Type> r, std::shared_ptr<CompoundStmt> s)
: name{new DeclName{n}}, params{p}, returnType{r} {}
