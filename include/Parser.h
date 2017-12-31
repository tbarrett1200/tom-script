#ifndef PARSER_H
#define PARSER_H

#include <deque>

#include "Token.h"
#include "Lexer.h"
#include "Tree.h"

class Parser {
  ErrorReporter &error;
  Lexer lexer;
  std::deque<Token> tokens;
public:
  Parser(std::string path, ErrorReporter &err);

  Token token(int index=0);
  void consume();
  void consumeUntil(std::vector<int> types);

  ParseTree parseOperator();
  ParseTree parseTerminal(int type, std::string str);

  ParseTree parseBasicValue();
  ParseTree parseIntegerLiteral();
  ParseTree parseIdentifier();

  ParseTree parseExpr();
  ParseTree parseBinaryExpr();

  ParseTree parseDecl();
  ParseTree parseVarDecl();
  ParseTree parseConstDecl();
  ParseTree parseParamDecl();
  ParseTree parseParamDeclList();
  ParseTree parseFuncDecl();

  ParseTree parseStmt();
  ParseTree parseIfStmt();
  ParseTree parseWhileStmt();
  ParseTree parseReturnStmt();
  ParseTree parseExprStmt();
  ParseTree parseDeclStmt();
  ParseTree parseStmtList();
  ParseTree parseBlockStmt();
};

#endif
