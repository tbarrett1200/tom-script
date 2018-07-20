#ifndef AST_TRANSFORMER_H
#define AST_TRANSFORMER_H

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"

template <typename T> class ASTTransformer {
public:
  virtual T transformIntegerExpr(const IntegerExpr&) = 0;
  virtual T transformDoubleExpr(const DoubleExpr &) = 0;
  virtual T transformBinaryExpr(const BinaryExpr &) = 0;

};

#endif
