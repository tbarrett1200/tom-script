
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <stack>

#include "Basic/SourceCode.h"
#include "Basic/CompilerException.h"
#include "Parse/Parser.h"

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    std::cout << "error: no file found" << std::endl;
    exit(1);
  }
  std::string path = argv[1];
  SourceManager::currentSource = new SourceCode(path);
  auto parser = Parser{SourceManager::currentSource};

  try {
    std::shared_ptr<StmtList> node = parser.parseStmtList();
  } catch (CompilerException e) {
    std::cerr << e << std::endl;
  }

  delete SourceManager::currentSource;
  return 0;
}
