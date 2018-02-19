#include "Lexer.h"
#include "Parser.h"

#include <iostream>
#include "Visitor.h"
#include "PrintVisitor.h"
#include "SymbolAnnotator.h"
#include "SymbolChecker.h"

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    std::cout << "error: no file found" << std::endl;
    exit(1);
  }
  std::string path = argv[1];
  SourceCode source{path};

  Program* node = Parser(&source).parseProgram();

  if (node!=nullptr) {
    PrintVisitor print{};
    node->accept(print);

    //SymbolAnnotator annotate{};
    //node->accept(annotate);
  //  SymbolChecker check{&source};
    //node->accept(check);
  }

  return 0;
}
