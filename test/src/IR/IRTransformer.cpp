#include <gtest/gtest.h>

#include <llvm/ADT/APInt.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>

#include <AST/Expr.h>
#include <IR/IRTransformer.h>

using namespace llvm;

TEST(IRTransformer, transformIntegerExpr) {
  IRTransformer transformer;
  IntegerExpr expr{5};
  expr.accept(transformer);
  llvm::ConstantInt* val = dyn_cast<llvm::ConstantInt>(transformer.getResult());
  ASSERT_TRUE(val->equalsInt(5));
}

TEST(IRTransformer, transformDoubleExpr) {
  IRTransformer transformer;
  DoubleExpr expr{5.0};
  expr.accept(transformer);
  llvm::ConstantFP* val = dyn_cast<llvm::ConstantFP>(transformer.getResult());
  ASSERT_TRUE(val->isExactlyValue(5.0));
}
