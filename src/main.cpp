#include "Lexer.h"
#include "Parser.h"

#include <iostream>

int main(int argc, char const *argv[]) {
  FileBuffer file{"test"};
  ErrorReporter err{file};
  Parser parse {"test", err};
  ParseTree node = parse.parseStmtList();
  std::cout << node;
  return 0;
}
