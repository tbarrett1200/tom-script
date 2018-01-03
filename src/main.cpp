#include "Lexer.h"
#include "Parser.h"

#include <iostream>
#include "Visitor.h"

int main(int argc, char const *argv[]) {
  FileBuffer file{"test"};
  ErrorReporter err{file};
  Parser parse {"test", err};
  StmtList* node = parse.parseStmtList();
  if (node==nullptr) return 0;
  PrintVisitor p{};
  node->accept(p);
  return 0;
}
