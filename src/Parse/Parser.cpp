#include "Parse/Parser.h"
#include "Basic/Token.h"
#include "Basic/SourceCode.h"
#include "Basic/ErrorReporter.h"
#include "Basic/CompilerException.h"

#include "Parse/Operator.h"

#include <assert.h>
#include <iostream>

Parser::Parser(std::shared_ptr<SourceFile> src) : source{src} {
  lexer = std::make_unique<Lexer>(src);
  token_ = lexer->next();
}


//=*****************************************************************************
//  # Utility
//=*****************************************************************************

void Parser::consumeUntil(std::vector<int> types) {
  while (!token_.isAny(types)) {
    consume();
  }
}

bool Parser::parseTerminal(int type, const char *lexeme, bool expect = true) {
  if (token_.is(type) && token_.lexeme() == StringRef{lexeme}) {
    consume();
    return true;
  } else {
    if (expect) {
      std::stringstream ss;
      ss << "expected " << lexeme << " but found '" << lexeme << "'";
      throw CompilerException(token_.location(), ss.str());
    }
    return false;
  }
}

bool Parser::consumeToken(int type) {
  if (token_.is(type)) {
    consume();
    return true;
  } else {
    return false;
  }
}

bool Parser::acceptToken(int type) {
  if (token_.is(type)) {
    return true;
  } else {
    return false;
  }
}

Token Parser::expectToken(int type, const char *lexeme) {
  Token tok = token_;
  if (tok.is(type)) {
    consume();
    return tok;

  } else {
    std::stringstream ss;
    ss << "expected " << lexeme << " but found '" << token_.lexeme() << "'";
    throw CompilerException(token_.location(), ss.str());
  }
}

Token Parser::expectToken(std::vector<int> types, const char *lexeme) {
  Token tok = token_;
  if (tok.isAny(types)) {
    consume();
    return tok;
  } else {
    std::stringstream ss;
    ss << "expected " << lexeme << " but found '" << tok.lexeme() << "'";
    throw CompilerException(tok.location(), ss.str());
  }
}

bool Parser::consumeOperator(const char *lexeme) {
  if (token_.is(Token::operator_id) && token_.lexeme() == StringRef{lexeme}) {
    consume();
    return true;
  } else {
    return false;
  }
}
