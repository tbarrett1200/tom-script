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
void IRTransformer::visit(const StringExpr &tree) {
  // TODO: implement
}
/**
 * A Visitor that generates LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const IntegerExpr &tree) {
  llvm::APInt val{64, (uint64_t)tree.getInt()};
  setResult(llvm::ConstantInt::get(*TheContext, val));
}
/**
 * A Visitor that generates LLVM IR from an Expression
 *
 */
void IRTransformer::visit(const DoubleExpr &tree) {
  llvm::APFloat val{tree.getDouble()};
  setResult(llvm::ConstantFP::get(*TheContext, val));
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
  tree.left->accept(*this);
  llvm::Value* lval = getResult();

  const Type* right = tree.right->getType()->getCanonicalType();
  tree.right->accept(*this);
  llvm::Value* rval = getResult();

  if (left->isIntegerType() && right->isIntegerType()) {
    if (tree.getOperator() == "+") {
      setResult( TheBuilder->CreateAdd(lval, rval));
    } else if (tree.getOperator() == "-") {
      setResult( TheBuilder->CreateSub(lval, rval));
    } else if (tree.getOperator() == "*") {
      setResult( TheBuilder->CreateMul(lval, rval));
    } else if (tree.getOperator() == "/") {
      setResult( TheBuilder->CreateSDiv(lval, rval));
    } else if (tree.getOperator() == "%") {
      setResult( TheBuilder->CreateSRem(lval, rval));
    } else {
      throw std::logic_error("error: binary expression of this type not implemented");
    }
  } else if (left->isDoubleType() && right->isDoubleType()) {
    if (tree.getOperator() == "+") {
      setResult( TheBuilder->CreateFAdd(lval, rval));
    } else if (tree.getOperator() == "-") {
      setResult( TheBuilder->CreateFSub(lval, rval));
    } else if (tree.getOperator() == "*") {
      setResult( TheBuilder->CreateFMul(lval, rval));
    } else if (tree.getOperator() == "/") {
      setResult( TheBuilder->CreateFDiv(lval, rval));
    } else {
      throw std::logic_error("error: binary expression of this type not implemented");
    }
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
