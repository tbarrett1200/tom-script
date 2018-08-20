#include <gtest/gtest.h>

#include "CodeGen/IRGenWalker.h"
#include "AST/Expr.h"

TEST(CodeGen, transformIntegerExpr) {
  llvm::LLVMContext context;
  llvm::Type *integer_type = llvm::Type::getInt64Ty(context);
  llvm::Value *integer_expr =  llvm::ConstantInt::get(integer_type, 5, true);
  ASSERT_TRUE(integer_expr);
}

TEST(CodeGen, transformDoubleExpr) {
  llvm::LLVMContext context;
  llvm::Type *double_type = llvm::Type::getDoubleTy(context);
  llvm::Value *double_expr =  llvm::ConstantFP::get(double_type, 5.0);
  ASSERT_TRUE(double_expr);
}

TEST(CodeGen, transformBooleanExpr) {
  llvm::LLVMContext context;
  llvm::Type *boolean_type = llvm::Type::getInt1Ty(context);
  llvm::Value *boolean_expr =  llvm::ConstantInt::get(boolean_type, 1, true);
  ASSERT_TRUE(boolean_expr);
}
