#ifndef LEXER_H
#define LEXER_H

#include <string>

#include "Basic/Token.h"
#include "Basic/SourceCode.h"

// Splits a source file into
class Lexer {
private:
  SourceCode *source;
  // lcation data for the current character in the file
  int cRow; //row
  int cCol; //col
  int cLoc; //index
  // location data for the start character in the file
  int sRow; //row
  int sCol; //col
  int sLoc; //index

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
public:
  Lexer(SourceCode*);
  Token next();
  Token getEOF();
};

#endif
