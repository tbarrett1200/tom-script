#include <iostream>
#include <sstream>
#include <string>

#include "Basic/SourceCode.h"
#include "Parse/Parser.h"
#include "AST/Type.h"
#include "AST/ASTWalker.h"
#include "AST/DeclContext.h"

DeclContext globalContext{
  Parser::makeDecl("func +(Int, Int) -> Int"),
  Parser::makeDecl("func +(Double, Double) -> Double"),
};

class TypePrinter : public ASTWalker {
public:
  bool visitDecl(std::shared_ptr<Decl> d) {
    d->setContext(&globalContext);
    globalContext.addDecl(d);
    try {
      auto type = d->getType();
      std::cout << d->getName() << ": " << type << std::endl;
    } catch (std::string s) {
      std::cout << s;
    }

    return true;
  }
};

class REPLoop {
  int count = 1;
public:
  void displayPrompt() {
    if (count < 10) {
      std::cout << "  \033[37m" << count << ">\033[0m ";
    } else if (count < 100) {
      std::cout << " \033[37m" << count << ">\033[0m ";
    } else {
      std::cout << "\033[37m" << count << ">\033[0m ";
    }
  }
  void runCommand(std::string line) {
    if (line == ":quit") {
      exit(0);
    } else if (line == ":help") {
      std::cout << "beep. boop. figure it out yourself nitwit!" << std::endl;
    } else {
      std::cout << "error: unrecognized command " << line << "... try ':help'" << std::endl;
    }
  }
  void start() {
    std::cout << "tom-script (0.1) Enter ':help' for help and ':quit' to quit" << std::endl;
    do {
      displayPrompt();
      std::string line;
      getline(std::cin, line);
      if (line[0] == ':') {
        runCommand(line);
      } else {
        const std::stringstream sstream{line};
        auto source = SourceCode{sstream, "terminal"};
        auto parser = Parser{&source};
        if (!parser.token().is(Token::eof)) {
          try {
            auto type = parser.parseDecl();
            TypePrinter().traverseDecl(type);
          } catch (std::string s) {
            std::cout << s;
          }
        }
      }
      count++;
    } while (!std::cin.eof());
  }
};

int main(int argc, char const *argv[]) {
  REPLoop().start();
  return 0;
}
