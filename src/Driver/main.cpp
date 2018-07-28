#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <stack>

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

#include "IR/KaleidoscopeJIT.h"
#include "IR/IRGenWalker.h"

#include "Basic/SourceCode.h"
#include "Basic/CompilerException.h"
#include "Basic/ErrorReporter.h"
#include "Parse/Parser.h"
#include "AST/ASTPrintWalker.h"
#include "AST/Decl.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"

bool printAST = false;
bool printIR = false;

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cout << "error: no file found" << std::endl;
    exit(1);
  }

  for (int i=2; i<argc; i++) {
    if (argv[i] == std::string("--printAST")) {
      printAST = true;
    } else if (argv[i] == std::string("--printIR")) {
      printIR = true;
    }
  }

  std::string path = argv[1];
  SourceManager::currentSource = std::make_shared<SourceCode>(path);
  auto parser = Parser{SourceManager::currentSource};

  try {
    std::shared_ptr<CompilationUnit> unit = parser.parseCompilationUnit();
    if (printAST) ASTPrintWalker(std::cout).traverse(unit);

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    llvm::LLVMContext TheContext;
    std::unique_ptr<llvm::Module> TheModule = llvm::make_unique<llvm::Module>("test", TheContext);
    std::unique_ptr<llvm::orc::KaleidoscopeJIT> TheJIT = llvm::make_unique<llvm::orc::KaleidoscopeJIT>();

    TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());

    LLVMTransformer transformer{TheContext, TheModule.get()};

   std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());
   TheFPM->add(llvm::createInstructionCombiningPass());
   TheFPM->add(llvm::createReassociatePass());
   TheFPM->add(llvm::createGVNPass());
   TheFPM->add(llvm::createCFGSimplificationPass());
   TheFPM->doInitialization();

    llvm::Function *llvmFunction;

    try {
      for (auto stmt: unit->getStmts()) {
        const DeclStmt *declStmt = stmt->as<const DeclStmt>();
        FuncDecl *funcDecl = declStmt->decl->as<FuncDecl>();
        llvmFunction = transformer.transformFunction(*funcDecl);
        verifyFunction(*llvmFunction);
        TheFPM->run(*llvmFunction);
      }
    } catch (const std::logic_error& e) {
      std::cout << e.what() << std::endl;
    } catch (const CompilerException& e2) {
      std::cout << e2.message << std::endl;
    }

    if (printIR) TheModule->print(llvm::errs(), nullptr);
    auto moduleHandle = TheJIT->addModule(std::move(TheModule));
    auto mainFunctionSymbol = TheJIT->findSymbol("main");
    int (*mainFunction)() = (int (*)())(intptr_t)cantFail(mainFunctionSymbol.getAddress());

    std::cout << "program returned successfully with value " << mainFunction() << std::endl;
    TheJIT->removeModule(moduleHandle);

  } catch (CompilerException e) {
    ErrorReporter{std::cout, *SourceManager::currentSource}.report(e);
  }

  return 0;
}
