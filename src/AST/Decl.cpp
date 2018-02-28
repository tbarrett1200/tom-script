#include "AST/Decl.h"
#include "AST/TypeExpr.h"

Decl::Decl(Token n) : name{n} {}

TypeAlias::TypeAlias(Token n, unique_ptr<Type> t)
: Decl{n}, type{move(t)} {
  if (!type) {
    throw domain_error("type-alias must specify type");
  }
}

VarDecl::VarDecl(Token n, unique_ptr<Type> t)
: Decl{n}, type{move(t)} {
  if (!type) {
    throw domain_error("var decl must specify type");
  }
}

VarDecl::VarDecl(Token n, unique_ptr<Expr> e)
: Decl{n}, expr{move(e)} {
  if (!expr) {
    throw domain_error("var decl must specify expr");
  }
}

LetDecl::LetDecl(Token n, unique_ptr<Expr> e)
: Decl{n}, expr{move(e)} {
  if (!expr) {
    throw domain_error("let decl must specify expr");
  }
}

LetDecl::LetDecl(Token n, unique_ptr<Type> t,  unique_ptr<Expr> e)
: Decl{n}, type{move(t)}, expr{move(e)} {
  if (!expr) {
    throw domain_error("let decl must specify type");
  }
}

FuncDecl::FuncDecl(Token n, unique_ptr<FunctionType> t)
: Decl{n}, type{move(t)} {
  if (!type) {
    throw domain_error("func decl must specify type");
  }
}
