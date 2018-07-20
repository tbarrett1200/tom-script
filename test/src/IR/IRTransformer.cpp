#include <gtest/gtest.h>

#include <AST/Expr.h>
#include <IR/ASTTransformer.h>
#include <IR/IRTransformer.h>

using namespace llvm;

TEST(IRTransformer, transformIntegerExpr) {
  IRTransformer transformer;
  IntegerExpr expr{5};
  llvm::ConstantInt* val = dyn_cast<llvm::ConstantInt>(transformer.transformIntegerExpr(expr));
  ASSERT_TRUE(val->equalsInt(5));
}

TEST(IRTransformer, transformDoubleExpr) {
  IRTransformer transformer;
  DoubleExpr expr{5.0};
  llvm::ConstantFP* val = dyn_cast<llvm::ConstantFP>(transformer.transformDoubleExpr(expr));
  ASSERT_TRUE(val->isExactlyValue(5.0));
}
