#include <gtest/gtest.h>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APFloat.h"
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
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include <AST/Expr.h>
#include <IR/IRGenWalker.h>
#include <IR/KaleidoscopeJIT.h>
/*
using namespace llvm;
using namespace llvm::orc;

class IRTransformerTest : public testing::Test {
public:

  LLVMContext TheContext;
  IRBuilder<> Builder{TheContext};
  std::unique_ptr<Module> TheModule;
  std::unique_ptr<legacy::FunctionPassManager> TheFPM;
  std::unique_ptr<KaleidoscopeJIT> TheJIT;

  virtual void SetUp() {
    // initial native setup
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    // JIT setup
    TheJIT = llvm::make_unique<KaleidoscopeJIT>();

    // module setup
    TheModule = llvm::make_unique<Module>("my cool jit", TheContext);
    TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());

    // optimization setup
    TheFPM = llvm::make_unique<legacy::FunctionPassManager>(TheModule.get());
    TheFPM->add(createInstructionCombiningPass());
    TheFPM->add(createReassociatePass());
    TheFPM->add(createGVNPass());
    TheFPM->add(createCFGSimplificationPass());
    TheFPM->doInitialization();
  }

  virtual void TearDown() {

  }

};


TEST_F(IRTransformerTest, transformIntegerExpr) {
  //--------------------------------------------------------------------------//
  llvm::FunctionType *FuncType = llvm::FunctionType::get(llvm::Type::getInt64Ty(TheContext), false);
  Function *TheFunction = Function::Create(FuncType, Function::ExternalLinkage, "test", TheModule.get());
  BasicBlock *BasicBlock = BasicBlock::Create(TheContext, "entry", TheFunction);
  Builder.SetInsertPoint(BasicBlock);
  //--------------------------------------------------------------------------//
  // create value here
  IRTransformer transformer;
  transformer.setContext(&TheContext);
  IntegerExpr expr{5};
  expr.accept(transformer);
  llvm::ConstantInt* val = dyn_cast<llvm::ConstantInt>(transformer.getResult());
  //--------------------------------------------------------------------------//
  Builder.CreateRet(val);
  verifyFunction(*TheFunction);
  TheFPM->run(*TheFunction);
  auto moduleHandle = TheJIT->addModule(std::move(TheModule));
  auto testFunctionSymbol = TheJIT->findSymbol("test");
  int (*testFunction)() = (int (*)())(intptr_t)cantFail(testFunctionSymbol.getAddress());
  //--------------------------------------------------------------------------//
  // test here
  ASSERT_EQ(testFunction(), 5);
  //--------------------------------------------------------------------------//
  TheJIT->removeModule(moduleHandle);
}

TEST_F(IRTransformerTest, transformDoubleExpr) {
  //--------------------------------------------------------------------------//
  llvm::FunctionType *FuncType = llvm::FunctionType::get(llvm::Type::getDoubleTy(TheContext), false);
  Function *TheFunction = Function::Create(FuncType, Function::ExternalLinkage, "test", TheModule.get());
  BasicBlock *BasicBlock = BasicBlock::Create(TheContext, "entry", TheFunction);
  Builder.SetInsertPoint(BasicBlock);
  //--------------------------------------------------------------------------//
  // create value here
  IRTransformer transformer;
  transformer.setContext(&TheContext);
  DoubleExpr expr{5.0};
  expr.accept(transformer);
  llvm::ConstantFP* val = dyn_cast<llvm::ConstantFP>(transformer.getResult());
  //--------------------------------------------------------------------------//
  Builder.CreateRet(val);
  verifyFunction(*TheFunction);
  TheFPM->run(*TheFunction);
  auto moduleHandle = TheJIT->addModule(std::move(TheModule));
  auto testFunctionSymbol = TheJIT->findSymbol("test");
  double (*testFunction)() = (double (*)())(intptr_t)cantFail(testFunctionSymbol.getAddress());
  //--------------------------------------------------------------------------//
  // test here
  ASSERT_EQ(testFunction(), 5.0);
  //--------------------------------------------------------------------------//
  TheJIT->removeModule(moduleHandle);
}

TEST_F(IRTransformerTest, transformBinaryExpr) {
  //--------------------------------------------------------------------------//
  llvm::FunctionType *FuncType = llvm::FunctionType::get(llvm::Type::getInt64Ty(TheContext), false);
  Function *TheFunction = Function::Create(FuncType, Function::ExternalLinkage, "test", TheModule.get());
  BasicBlock *BasicBlock = BasicBlock::Create(TheContext, "entry", TheFunction);
  Builder.SetInsertPoint(BasicBlock);
  //--------------------------------------------------------------------------//
  // create value here
  IRTransformer transformer;
  transformer.setBuilder(&Builder);
  transformer.setContext(&TheContext);
  std::shared_ptr<IntegerExpr> left = std::make_shared<IntegerExpr>(5);
  std::shared_ptr<IntegerExpr> right = std::make_shared<IntegerExpr>(10);
  Token op{"+", Token::operator_id, 0, 0, 0};
  std::shared_ptr<BinaryExpr> result = std::make_shared<BinaryExpr>(left, op, right, left->getType());
  result->accept(transformer);
  llvm::ConstantInt* val = dyn_cast<llvm::ConstantInt>(transformer.getResult());
  //--------------------------------------------------------------------------//
  Builder.CreateRet(val);
  verifyFunction(*TheFunction);
  TheFPM->run(*TheFunction);
  auto moduleHandle = TheJIT->addModule(std::move(TheModule));
  auto testFunctionSymbol = TheJIT->findSymbol("test");
  int (*testFunction)() = (int (*)())(intptr_t)cantFail(testFunctionSymbol.getAddress());
  //--------------------------------------------------------------------------//
  // test here
  ASSERT_EQ(testFunction(), 15);
  //--------------------------------------------------------------------------//
  TheJIT->removeModule(moduleHandle);

}
*/
