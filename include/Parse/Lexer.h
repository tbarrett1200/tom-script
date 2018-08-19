#ifndef LEXER_H
#define LEXER_H

#include <string>

#include "Basic/Token.h"
#include "Basic/SourceCode.h"

// Splits a source file into
class Lexer {
private:
  std::shared_ptr<SourceFile> source;
  std::string::const_iterator source_iterator;

  Token lexIdentifier();
  Token lexNumber();
  Token lexStringLiteral();
  Token lexPunctuation();
  Token lexCharacterLiteral();
  Token lexOperatorIdentifier();
  void lexSlashStarComment();
  void lexSlashSlashComment();
public:
  Lexer(std::shared_ptr<SourceFile> source);
  const char* current_loc() const;
  Token next();
};

#endif
