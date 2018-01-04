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

  Operator* parseOperator();
  bool parseTerminal(int type, std::string str, bool expect);

  Expr* parseExpr();
  Expr* parseBinaryExpr();
  Expr* parseValueExpr();
  IntegerLiteral* parseIntegerLiteral();
  Identifier* parseIdentifier();

  VarDecl* parseVarDecl();
  VarDecl* parseParamDecl();
  StmtList* parseParamList();
  FuncDecl* parseFuncDecl();

  Stmt* parseStmt();
  IfStmt* parseIfStmt();
  WhileStmt* parseWhileStmt();
  ReturnStmt* parseReturnStmt();
  ExprStmt* parseExprStmt();
  StmtList* parseStmtList();
  BlockStmt* parseBlockStmt();
};

#endif
