#ifndef LEXER_H
#define LEXER_H

#include <string>

#include "Token.h"
#include "ErrorReporter.h"

class Lexer {
  ErrorReporter &error;
  std::string file;
  int cRow;
  int cCol;
  int cLoc;
  int sRow;
  int sCol;
  int sLoc;
public:
  Lexer(std::string path, ErrorReporter &err);
  Token next();
private:
  void advance();
  bool advanceIf(bool);
  char at(int index = 0);
  Token lexIdentifier();
  Token lexNumber();
  Token lexStringLiteral();
  Token lexCharLiteral();
  Token lexOperatorIdentifier();
  void skipSlashStarComment();
  void skipSlashSlashComment();
};

#endif
