#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include "IR/IRTransformer.h"
#include "AST/Expr.h"
#include "AST/Type.h"

static llvm::LLVMContext TheContext;

/**
 * A Visitor that generates LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const Expr &tree) {
  // TODO: implement
}
/**
 * A Visitor that generates LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const LabeledExpr &tree) {
  // TODO: implement
}
/**
 * A Visitor that generates LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const StringExpr &tree) {
  // TODO: implement
}
/**
 * A Visitor that generates LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const IntegerExpr &tree) {
  llvm::APInt val{64, (uint64_t)tree.getInt()};
  setResult(llvm::ConstantInt::get(TheContext, val));
}
/**
 * A Visitor that generates LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const DoubleExpr &tree) {
  llvm::APFloat val{tree.getDouble()};
  setResult(llvm::ConstantFP::get(TheContext, val));
}
/**
 * A Visitor that generates LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const ListExpr&tree) {
  // TODO: implement
}
/**
 * A Visitor that generates LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const IdentifierExpr &tree) {
  // TODO: implement
}

/**
 * A Visitor that generation LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const TupleExpr &tree) {
  // TODO: Implement
}
/**
 * A Visitor that generation LLVM IR from an Expression
 *
 */
void  IRTransformer::visit(const AccessorExpr &tree) {
  // TODO: Implement
}
/**
 * A Visitor that generation LLVM IR from an Expression
 *
 */
void  IRTransformer::visit(const OperatorExpr&tree) {
  // TODO: Implement
}
/**
 * A Visitor that generation LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const BoolExpr &tree) {
  // TODO: Implement
}
/**
 * A Visitor that generation LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const UnaryExpr &tree) {
  // TODO: Implement
}
/**
 * A Visitor that generation LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const BinaryExpr &tree) {
  const Type* left = tree.left->getType()->getCanonicalType();
  const Type* right = tree.left->getType()->getCanonicalType();
  if (left->isIntegerType() && right->isIntegerType()) {
    throw std::logic_error("error: binary expression of this type not implemented");
  } else {
    throw std::logic_error("error: binary expression of this type not implemented");
  }
}
/**
 * A Visitor that generation LLVM IR from an Expression
 *
 */
void  IRTransformer::visit(const FunctionCall &tree) {
  // TODO: Implement
}
/**
 * A Visitor that generation LLVM IR from an Expression
 *
 */
void  IRTransformer::visit(const StackPointer &tree) {
  // TODO: Implement
}
