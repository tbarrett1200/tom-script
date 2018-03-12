#include <iostream>
#include <sstream>
#include <string>

#include "Basic/SourceCode.h"
#include "Parse/Parser.h"
#include "AST/Type.h"

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
          auto type = parser.parseDecl();
          if (type && !parser.token().is(Token::eof)) {
            parser.report(parser.token(), "error: expected eof");
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
