#include <iostream>
#include <sstream>
#include <string>

#include "Basic/SourceCode.h"
#include "Parse/Parser.h"
#include "AST/TypeExpr.h"

class REPLoop {
  int count = 1;
public:
  void displayPrompt() {
    if (count < 10) {
      std::cout << "  " << count << "> ";
    } else if (count < 100) {
      std::cout << " " << count << "> ";
    } else {
      std::cout << count << "> ";
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
    std::cout << "tom-script (0.0.0.0.1) Enter ':help' for help and ':quit' to quit" << std::endl;
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
          auto type = parser.parseDecl();
        }
        if (!parser.token().is(Token::eof)) {
          parser.report(parser.token(), "error: expected eof");
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
