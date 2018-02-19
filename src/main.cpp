#include "Lexer.h"
#include "Parser.h"

#include <iostream>
#include "Visitor.h"
#include "PrintVisitor.h"
#include "SymbolChecker.h"
#include "TypeChecker.h"

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    std::cout << "error: no file found" << std::endl;
    exit(1);
  }
  std::string path = argv[1];
  SourceCode source{path};

  Program* node = Parser(&source).parseProgram();

  if (node!=nullptr) {
    node->defineSymbolTable();
    SymbolChecker check1{&source};
    node->accept(check1);
    TypeChecker check2{&source};
    node->accept(check2);
  }

  return 0;
}
