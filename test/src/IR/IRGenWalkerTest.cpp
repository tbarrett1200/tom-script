#include <gtest/gtest.h>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include <iostream>
#include <AST/Expr.h>
#include <AST/Decl.h>
#include <AST/Stmt.h>
#include <AST/Type.h>
#include <Parse/Parser.h>
#include <IR/IRGenWalker.h>
#include <IR/KaleidoscopeJIT.h>
#include "AST/ASTXMLPrintWalker.h"

using namespace llvm;
using namespace llvm::orc;

TEST(IRGenWalker, transformFunction) {
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  std::unique_ptr<KaleidoscopeJIT> TheJIT = llvm::make_unique<KaleidoscopeJIT>();
  LLVMTransformer transformer{"test"};
  transformer.getModule()->setDataLayout(TheJIT->getTargetMachine().createDataLayout());

   std::unique_ptr<legacy::FunctionPassManager> TheFPM = llvm::make_unique<legacy::FunctionPassManager>(transformer.getModule());
   TheFPM->add(createInstructionCombiningPass());
   TheFPM->add(createReassociatePass());
   TheFPM->add(createGVNPass());
   TheFPM->add(createCFGSimplificationPass());
   TheFPM->doInitialization();

  SourceManager::currentSource = new SourceCode("test_001");
  Parser parser = Parser{SourceManager::currentSource};
  std::shared_ptr<FuncDecl> function = parser.parseFuncDecl();

  XMLPrintWalker(std::cout).traverse(function);

  llvm::Function *llvmFunction = transformer.transformFunction(*function);

  verifyFunction(*llvmFunction);

  transformer.getModule()->print(errs(), nullptr);
  TheFPM->run(*llvmFunction);
  auto moduleHandle = TheJIT->addModule(transformer.getModuleOwnership());
  auto testFunctionSymbol = TheJIT->findSymbol("main");
  int (*testFunction)() = (int (*)())(intptr_t)cantFail(testFunctionSymbol.getAddress());
  //--------------------------------------------------------------------------//
  // test here
  ASSERT_EQ(testFunction(), 4);
  //--------------------------------------------------------------------------//
  TheJIT->removeModule(moduleHandle);
}
