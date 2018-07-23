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


//=--------------------------------------------------------------------------=//
// TypeAlias
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<TreeElement>> TypeAlias::getChildren() const {
  return {};
}
std::shared_ptr<Expr> TypeAlias::getExpr() const { return nullptr; };
void TypeAlias::setExpr(std::shared_ptr<Expr> e) {};
Decl::Kind TypeAlias::getKind() const { return Kind::TypeAlias; }
std::string TypeAlias::getName() const { return name.lexeme; }

TypeAlias::TypeAlias(Token n, std::shared_ptr<Type> t)
: name{n}, type{move(t)} {}

//=--------------------------------------------------------------------------=//
// VarDecl
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::vector<std::shared_ptr<TreeElement>> VarDecl::getChildren() const {
  return {expr};
}

std::shared_ptr<Expr> VarDecl::getExpr() const { return expr; }

Decl::Kind VarDecl::getKind() const { return Kind::VarDecl; }
std::string VarDecl::getName() const { return name.lexeme; }

VarDecl::VarDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e)
: name{n}, type{move(t)}, expr{move(e)} {}

//=--------------------------------------------------------------------------=//
// LetDecl
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<TreeElement>> LetDecl::getChildren() const {
  return {expr};
}

Decl::Kind LetDecl::getKind() const { return Kind::LetDecl; }
std::string LetDecl::getName() const { return name.lexeme; }

void LetDecl::setExpr(std::shared_ptr<Expr> e) {
  expr = e;
}
std::shared_ptr<Expr> LetDecl::getExpr() const { return expr; }

LetDecl::LetDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e)
: name{n}, type{move(t)}, expr{move(e)} {
  if (!expr) {
    throw std::domain_error("let decl must specify type");
  }
}

//=--------------------------------------------------------------------------=//
// ParamDecl
//=--------------------------------------------------------------------------=//


Decl::Kind ParamDecl::getKind() const { return Decl::Kind::ParamDecl; }
std::string ParamDecl::getName() const { return name.lexeme; }

std::vector<std::shared_ptr<TreeElement>> ParamDecl::getChildren() const {
  return { };
}

//=--------------------------------------------------------------------------=//
// FuncDecl
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<TreeElement>> FuncDecl::getChildren() const {
  std::vector<std::shared_ptr<TreeElement>> children;
  for (auto param: fParams) {
    children.push_back(param);
  }
  children.push_back(fStmt);
  return children;
}

Decl::Kind FuncDecl::getKind() const { return Kind::FuncDecl; }
std::string FuncDecl::getName() const { return fName.lexeme; }
