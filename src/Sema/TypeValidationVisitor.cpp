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
//  tree.setType(StringType::getInstance());

}
/**
 *
 */
void TypeValidationVisitor::visit(const IntegerExpr &tree) {
  //tree.setType(IntegerType::getInstance());
}
/**
 *
 */
void TypeValidationVisitor::visit(const DoubleExpr &tree) {
  //tree.setType(DoubleType::getInstance());
}
/**
 *
 */
void TypeValidationVisitor::visit(const ListExpr &tree) {
  // cType *common = nullptr;
  // for (auto e: tree.getElements()) {
  //   e->accept(*this);
  //   if (common == nullptr) {
  //     common = e->getType()->getCanonicalType();
  //   } else if (common != e->getType()->getCanonicalType()) {
  //     throw std::logic_error('list element types are inconsistent');
  //   }
  // }
  // tree.setType(ListType::getInstance(common));
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
