#include "Sema/TypeValidationVisitor.h"
#include "Basic/CompilerException.h"
#include "AST/Expr.h"
#include "AST/Type.h"

void TypeValidationVisitor::visit(const Expr &tree) {
  // TODO
}
/**
 *
 */
void TypeValidationVisitor::visit(const LabeledExpr &tree) {
  throw std::logic_error(
    "labeled expressions not yet supported"
  );
}
/**
 *
 */
void TypeValidationVisitor::visit(const StringExpr &tree) {
  throw std::logic_error(
    "string expressions not yet supported"
  );
}
/**
 *
 */
void TypeValidationVisitor::visit(const IntegerExpr &tree) {
  if (!tree.getType()->isIntegerType()) throw std::logic_error(
    "integer expression does not have an integer type"
  );
}
/**
 *
 */
void TypeValidationVisitor::visit(const DoubleExpr &tree) {
  if(!tree.getType()->isDoubleType()) throw std::logic_error(
    "double expression does not have an double type"
  );
}
/**
 *
 */
void TypeValidationVisitor::visit(const ListExpr &tree) {
  // TODO
}
/**
 *
 */
void TypeValidationVisitor::visit(const IdentifierExpr &tree) {

}
/**
 *
 */
void TypeValidationVisitor::visit(const TupleExpr &tree) {
  // TODO
}
/**
 *
 */
void TypeValidationVisitor::visit(const AccessorExpr &tree) {
  // TODO
}
/**
 *
 */
void TypeValidationVisitor::visit(const OperatorExpr &tree) {
  // TODO
}
/**
 *
 */
void TypeValidationVisitor::visit(const BoolExpr &tree) {

}
/**
 *
 */
void TypeValidationVisitor::visit(const UnaryExpr &tree) {
  // TODO
}
/**
 *
 */
void TypeValidationVisitor::visit(const BinaryExpr &tree) {
  // TODO
}
/**
 *
 */
void TypeValidationVisitor::visit(const FunctionCall &tree) {
  // TODO
}
/**
 *
 */
void TypeValidationVisitor::visit(const StackPointer &tree) {
  // TODO
}
