#include "Parse/Parser.h"
#include "Basic/Token.h"
#include "Basic/ErrorReporter.h"
#include "Basic/CompilerException.h"

#include "Parse/Operator.h"

#include <assert.h>
#include <iostream>

Parser::Parser(std::shared_ptr<SourceCode> src) : source{src} {
  lexer = std::make_unique<Lexer>(src);
}
Parser::Parser(std::string s, std::string name) {
  source = std::make_shared<SourceCode>(std::istringstream{s}, name);
  lexer = std::make_unique<Lexer>(source);
}

//=*****************************************************************************
//  # Utility
//=*****************************************************************************


Token Parser::token(int index) {
  while (index >= tokens.size()) {
    tokens.push_back(lexer->next());
  }
  return tokens.at(index);
}

auto Parser::consume() -> void {
  if (tokens.size() > 0) {
    return tokens.pop_front();
  }
}

void Parser::consumeUntil(std::vector<int> types) {
  while (!token().isAny(types)) {
    consume();
  }
}

bool Parser::parseTerminal(int type, std::string str, bool expect = true) {
  Token tok = token();
  if (tok.is(type) && tok.lexeme == str) {
    consume();
    return true;
  } else {
    if (expect) {
      throw CompilerException(tok.getLocation(), "expected " + str + " but found '" + tok.lexeme + "'");
    }
    return false;
  }
}

bool Parser::consumeToken(int type) {
  Token tok = token();
  if (tok.is(type)) {
    consume();
    return true;
  } else {
    return false;
  }
}

bool Parser::acceptToken(int type) {
  if (token().is(type)) {
    return true;
  } else {
    return false;
  }
}

Token Parser::expectToken(int type, std::string name) {
  Token tok = token();
  if (tok.is(type)) {
    consume();
    return tok;
  } else throw CompilerException(token().getLocation(), "expected " + name + " but found '" + tok.lexeme + "'");
}

Token Parser::expectToken(std::vector<int> types, std::string name) {
  Token tok = token();
  if (tok.isAny(types)) {
    consume();
    return tok;
  } else throw CompilerException(token().getLocation(), "expected " + name + " but found '" + tok.lexeme + "'");
}

bool Parser::consumeOperator(std::string s) {
  Token tok = token();
  if (tok.is(Token::operator_id) && tok.lexeme == s) {
    consume();
    return true;
  } else {
    return false;
  }
}
