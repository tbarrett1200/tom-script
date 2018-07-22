#ifndef SEMA_TYPE_VALIDATION_VISITOR
#define SEMA_TYPE_VALIDATION_VISITOR

#include "AST/ASTVisitor.h"

class TypeValidationVisitor: public ASTVisitor, public ASTVisitorResult<bool> {
  void visit(const class Expr&) override;
  void visit(const class LabeledExpr&) override;
  void visit(const class StringExpr &) override;
  void visit(const class IntegerExpr &) override;
  void visit(const class DoubleExpr &) override;
  void visit(const class ListExpr&) override;
  void visit(const class IdentifierExpr &) override;
  void visit(const class TupleExpr &) override;
  void visit(const class AccessorExpr &) override;
  void visit(const class BoolExpr &) override;
  void visit(const class UnaryExpr &) override;
  void visit(const class BinaryExpr &) override;
  void visit(const class FunctionCall&) override;
  void visit(const class StackPointer &) override;
};

#endif
