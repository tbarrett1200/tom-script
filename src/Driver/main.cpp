#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <stack>

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
    }
  }

  std::string path = argv[1];
  SourceManager::currentSource = std::make_shared<SourceCode>(path);
  auto parser = Parser{SourceManager::currentSource};


  try {
    std::shared_ptr<CompilationUnit> unit = parser.parseCompilationUnit();
    ScopeBuilder().buildCompilationUnitScope(*unit);
    if (printAST) {
      std::ofstream myfile;
      myfile.open ("/Users/thomasbarrett/Desktop/app/tree.json");
      printASTNode(myfile, unit.get());
      myfile.close();
    }
    if (printScope) ASTScopePrinter(std::cout).traverse(unit);
  } catch (CompilerException e) {
    ErrorReporter{std::cout, *SourceManager::currentSource}.report(e);
  }

  return 0;
}
