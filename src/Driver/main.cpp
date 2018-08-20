#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <stack>
#include <system_error>

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

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "CodeGen/KaleidoscopeJIT.h"
#include "CodeGen/IRGenWalker.h"

#include "Basic/SourceCode.h"
#include "Basic/CompilerException.h"
#include "Basic/ErrorReporter.h"
#include "Parse/Parser.h"
#include "AST/ASTPrintWalker.h"
#include "Sema/ScopeBuilder.h"
#include "Sema/ASTScopePrinter.h"

#include "AST/Decl.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"

bool printAST = false;
bool printScope = false;
bool printIR = false;
bool Onone = false;
bool JIT = false;

void compileAST(CompilationUnit& unit);
int compile_to_object_code(CompilationUnit& unit);

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
    } else if (argv[i] == std::string("--printScope")) {
      printScope = true;
    } else if (argv[i] == std::string("--Onone")) {
      Onone = true;
    } else if (argv[i] == std::string("--JIT")) {
      JIT = true;
    }
  }

  std::string path = argv[1];
  SourceManager::currentSource = std::make_shared<SourceFile>(path);
  auto parser = Parser{SourceManager::currentSource};


  try {
    std::unique_ptr<CompilationUnit> unit = parser.parseCompilationUnit();
    ScopeBuilder().buildCompilationUnitScope(*unit);
    if (printAST) {
      std::ofstream myfile;
      myfile.open ("/Users/thomasbarrett/Desktop/app/tree.json");
      ASTPrintWalker{myfile}.traverse(unit.get());
      myfile.seekp((int)myfile.tellp()-1);
      myfile << " ";
      myfile.close();
    }
    if (printScope) ASTScopePrinter(std::cout).traverse(unit.get());
    if (JIT) compileAST(*unit); else return compile_to_object_code(*unit);
  } catch (CompilerException e) {
      ErrorReporter{std::cout, *SourceManager::currentSource}.report(e);
  }
  return 0;
}

// instructions for compilation
// 1 ./bin/tomscript test/test_data/MathLibTest
// 2 ld output.o -e _main -macosx_version_min 10.13 -lSystem -lc
// 3 ./a.out
int compile_to_object_code(CompilationUnit& unit) {
  // Initialize the target registry etc.
 llvm::InitializeAllTargetInfos();
 llvm::InitializeAllTargets();
 llvm::InitializeAllTargetMCs();
 llvm::InitializeAllAsmParsers();
 llvm::InitializeAllAsmPrinters();

 llvm::LLVMContext TheContext;
 std::unique_ptr<llvm::Module> TheModule = llvm::make_unique<llvm::Module>("test", TheContext);

 LLVMTransformer transformer{TheContext, TheModule.get()};
 llvm::Function *llvmFunction;

 for (auto &stmt: unit.stmts()) {
   if (const DeclStmt *declStmt = dynamic_cast<const DeclStmt*>(stmt.get())) {
     if (const FuncDecl *func_decl = dynamic_cast<const FuncDecl*>(declStmt->getDecl())) {
       llvmFunction = transformer.transformFunction(*func_decl);
     } else if (const ExternFuncDecl *func_decl = dynamic_cast<const ExternFuncDecl*>(declStmt->getDecl())) {
       llvmFunction = transformer.transformExternalFunctionDecl(*func_decl);
     } else throw CompilerException(nullptr, "only func decl allowed in top level code");
     verifyFunction(*llvmFunction);
   } else throw CompilerException(nullptr, "only func decl allowed in top level code");

 }

 auto TargetTriple = llvm::sys::getDefaultTargetTriple();
 TheModule->setTargetTriple(TargetTriple);

 std::string Error;
 auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

 // Print an error and exit if we couldn't find the requested target.
 // This generally occurs if we've forgotten to initialise the
 // TargetRegistry or we have a bogus target triple.
 if (!Target) {
   llvm::errs() << Error;
   return 1;
 }

 auto CPU = "generic";
 auto Features = "";

 llvm::TargetOptions opt;
 auto RM = llvm::Optional<llvm::Reloc::Model>();
 auto TheTargetMachine =
     Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

 TheModule->setDataLayout(TheTargetMachine->createDataLayout());

 auto Filename = "./output.o";
 std::error_code EC;
 llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::F_None);

 std::error_code err_code;
 llvm::raw_fd_ostream ir_stream{llvm::StringRef{"/Users/thomasbarrett/Desktop/app/tree.txt"}, err_code,  llvm::sys::fs::F_None };
 if (printIR) TheModule->print(ir_stream, nullptr);

 if (EC) {
   llvm::errs() << "Could not open file: " << EC.message();
   return 1;
 }

 llvm::legacy::PassManager pass;
 auto FileType = llvm::TargetMachine::CGFT_ObjectFile;

 if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
   llvm::errs() << "TheTargetMachine can't emit a file of this type";
   return 1;
 }

 pass.run(*TheModule);
 dest.flush();

 llvm::outs() << "Wrote " << Filename << "\n";

 return 0;
}

void compileAST(CompilationUnit& unit) {
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
      for (auto &stmt: unit.stmts()) {
        const DeclStmt *declStmt = dynamic_cast<const DeclStmt*>(stmt.get());
        const FuncDecl *funcDecl = dynamic_cast<const FuncDecl*>(declStmt->getDecl());
        llvmFunction = transformer.transformFunction(*funcDecl);
        verifyFunction(*llvmFunction);
        if (!Onone) TheFPM->run(*llvmFunction);
      }

      std::error_code err_code;
      llvm::raw_fd_ostream ir_stream{llvm::StringRef{"/Users/thomasbarrett/Desktop/app/tree.txt"}, err_code,  llvm::sys::fs::F_None };
      if (printIR) TheModule->print(ir_stream, nullptr);
      auto moduleHandle = TheJIT->addModule(std::move(TheModule));
      auto mainFunctionSymbol = TheJIT->findSymbol("main");
      int (*mainFunction)() = (int (*)())(intptr_t)cantFail(mainFunctionSymbol.getAddress());

      std::cout << "program returned successfully with value " << mainFunction() << std::endl;
      TheJIT->removeModule(moduleHandle);

    } catch (const std::logic_error& e) {
      std::cout << e.what() << std::endl;
    } catch (const CompilerException& e2) {
      std::cout << e2.message << std::endl;
    }

}
