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

llvm::Value* IRTransformer::transformIntegerExpr(const IntegerExpr &tree) {
  return llvm::ConstantInt::get(TheContext, llvm::APInt(64, (uint64_t)tree.getInt()));
}

llvm::Value* IRTransformer::transformDoubleExpr(const DoubleExpr &tree) {
  return llvm::ConstantFP::get(TheContext, llvm::APFloat(tree.getDouble()));
}

llvm::Value* IRTransformer::transformBinaryExpr(const BinaryExpr &tree) {
  const Type* left = tree.left->getType()->getCanonicalType();
  const Type* right = tree.left->getType()->getCanonicalType();
  if (left->isIntegerType() && right->isIntegerType()) {
    throw std::logic_error("error: binary expression of this type not implemented");
  } else {
    throw std::logic_error("error: binary expression of this type not implemented");
  }
}
