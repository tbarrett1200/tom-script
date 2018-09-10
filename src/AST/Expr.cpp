#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/Decl.h"

bool UnaryExpr::isLeftValue() const {
  return getType()->getKind() == Type::Kind::ReferenceType;
}

bool IdentifierExpr::isLeftValue() const {
  std::cout << decl_->name() << std::endl;
  return decl_->is<const VarDecl>() || decl_->is<const LetDecl>() || decl_->is<const UninitializedVarDecl>();
}
