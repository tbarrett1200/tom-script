#include <iostream>
#include <sstream>
#include <string>
#include <stack>

#include "Basic/SourceCode.h"
#include "Parse/Parser.h"
#include "AST/ASTWalker.h"
#include "Driver/GlobalContext.h"
#include "Sema/TypeChecker.h"
#include "Sema/Interpreter.h"


int main(int argc, char const *argv[]) {
  std::cout << "tom-script (0.1) Enter ':help' for help and ':quit' to quit" << std::endl;
  auto source = SourceCode{};
  auto parser = Parser{&source};
  auto semantic = TypeChecker();
  auto interpreter = Interpreter();
  do {
    if (!parser.token().is(Token::eof)) {
      try {
        auto stmt = parser.parseStmt();
        try {
          semantic.traverse(stmt);
          interpreter.traverse(stmt);
        } catch (std::string s) {
          std::cout << s;
        }
      } catch (std::string s) {
        std::cout << s;
        parser.consumeUntil({Token::new_line, Token::eof});
        parser.consume();
      }
      source.reset();
    }
  } while (!std::cin.eof());
  return 0;
}
