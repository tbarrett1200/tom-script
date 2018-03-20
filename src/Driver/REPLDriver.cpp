#include <iostream>
#include <sstream>
#include <string>

#include "Basic/SourceCode.h"
#include "Parse/Parser.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/ASTWalker.h"
#include "AST/DeclarationContext.h"
#include "Driver/GlobalContext.h"

class Interpreter : public ASTWalker {
public:
  DeclarationContext *context = &globalContext;

  bool visitConditionalStmtList(std::shared_ptr<ConditionalStmtList> s) {
    return true;
  }

  bool visitWhileLoop(std::shared_ptr<WhileLoop> l) {
    return false;
  }

  bool visitExprStmt(std::shared_ptr<ExprStmt> e) {
    try {
      std::cout << e->expr->getType(context);
    } catch (std::string s) {
      std::cout << s;
    }
    return false;
  }

  bool visitDeclStmt(std::shared_ptr<DeclStmt> d) {
    try {
      d->decl->setContext(&globalContext);
      std::cout << d->decl->getType() << std::endl;
      globalContext.add(d->decl);
    } catch (std::string s) {
      std::cout << s;
    }
    return false;
  }
};

int main(int argc, char const *argv[]) {
  std::cout << "tom-script (0.1) Enter ':help' for help and ':quit' to quit" << std::endl;
  auto source = SourceCode{};
  auto parser = Parser{&source};
  do {
    if (!parser.token().is(Token::eof)) {
      try {
        auto type = parser.parseStmt();
        Interpreter().traverse(type);
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
