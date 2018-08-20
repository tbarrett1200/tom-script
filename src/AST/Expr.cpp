#include "AST/Expr.h"
#include "AST/Type.h"

bool UnaryExpr::isLeftValue() const {
  return getType()->getKind() == Type::Kind::ReferenceType;
}
