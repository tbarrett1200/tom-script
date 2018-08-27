#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/Decl.h"

bool UnaryExpr::isLeftValue() const {
  return getType()->getKind() == Type::Kind::ReferenceType;
}

bool IdentifierExpr::isLeftValue() const {
  return decl_->is<const VarDecl>();
}
