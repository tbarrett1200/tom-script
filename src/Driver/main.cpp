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

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    std::cout << "error: no file found" << std::endl;
    exit(1);
  }
  std::string path = argv[1];
  SourceManager::currentSource = std::make_shared<SourceCode>(path);
  auto parser = Parser{SourceManager::currentSource};

  try {
    std::vector<std::shared_ptr<Stmt>> node = parser.parseStmtList();
    for (auto stmt: node) {
      ASTPrintWalker(std::cout).traverse(stmt);
    }
  } catch (CompilerException e) {
    ErrorReporter{std::cout, *SourceManager::currentSource}.report(e);
  }

  return 0;
}
