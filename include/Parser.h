#ifndef PARSER_H
#define PARSER_H

#include <deque>

#include "Token.h"
#include "Lexer.h"
#include "Tree.h"

class Parser {
  Lexer lexer;
  std::deque<Token> tokens;
public:
  Parser(std::string path);
  Token token(int index=0);
  void consume();
  bool consumeIf(bool);
  ParseTree parseIntegerLiteral();
  ParseTree parseIdentifier();
  ParseTree parseOperator();
  ParseTree parseBinaryExpr();
  ParseTree parseExpr();
  ParseTree parseDecl();
  ParseTree parseVarDecl();
  ParseTree parseConstDecl();
  ParseTree parseParamDecl();
  ParseTree parseParamDeclList();
  ParseTree parseStmt();
  ParseTree parseIfStmt();
  ParseTree parseWhileStmt();
  ParseTree parseReturnStmt();
  ParseTree parseExprStmt();
  ParseTree parseDeclStmt();
  ParseTree parseFuncDecl();
  ParseTree parseStmtList();
  ParseTree parseBlockStmt();
};

#endif
