#include "Lexer.h"
#include "Parser.h"

#include <iostream>

int main(int argc, char const *argv[]) {
  Parser parse {"test"};
  ParseTree node = parse.parseStmtList();
  std::cout << node;
  return 0;
}
