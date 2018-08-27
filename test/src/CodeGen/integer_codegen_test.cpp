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
// tests producing the example code in llvm
// void main() {
//   char a[10];
//   char *b = a;
//   char c = b[0];
// }
TEST(CodeGen, transformAccessorExpr) {
  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> module = llvm::make_unique<llvm::Module>("test", context);
  llvm::Type *double_type = llvm::Type::getDoubleTy(context);
  llvm::FunctionType *func_type = llvm::FunctionType::get(double_type, {});
  llvm::Function *func = llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage, "test_func", module.get());
  llvm::BasicBlock *block = llvm::BasicBlock::Create(context, "test_func_entry", func);
  llvm::IRBuilder<> builder{block};

  llvm::ArrayType *array_type = llvm::ArrayType::get(llvm::Type::getDoubleTy(context), 3);

  llvm::Value *array_value = llvm::ConstantArray::get(array_type, {
    llvm::ConstantFP::get(double_type, 2.0)
  , llvm::ConstantFP::get(double_type, 3.0)
  , llvm::ConstantFP::get(double_type, 4.0)
  });

  llvm::AllocaInst *array = builder.CreateAlloca(array_type);
  builder.CreateStore(array_value, array);

  llvm::Type *array_ref_type = llvm::PointerType::getUnqual(double_type);
  llvm::AllocaInst *array_ref = builder.CreateAlloca(array_ref_type);

  builder.CreateStore(array, array_ref);

  llvm::Type *index_type = llvm::Type::getInt32Ty(context);

  llvm::Value *first_element = builder.CreateGEP(array_ref, {
    llvm::ConstantInt::get(index_type, 0)
  });

  ASSERT_TRUE(first_element);
}
