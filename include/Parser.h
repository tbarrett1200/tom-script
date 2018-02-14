#ifndef PARSER_H
#define PARSER_H

#include <deque>

#include "Token.h"
#include "Lexer.h"
#include "Tree.h"

class Parser {
  SourceCode *source;
  Lexer lexer;
  std::deque<Token> tokens;
public:
  Parser(SourceCode *source);

  Token token(int index=0);
  void consume();
  void consumeUntil(std::vector<int> types);

  Operator* parseOperator();
  bool parseTerminal(int type, std::string str, bool expect);

  Expr* parseExpr();
  Expr* parseBinaryExpr();
  Expr* parseValueExpr();
  ExprList* parseExprList();
  FunctionCall* parseFunctionCall();
  IntLiteral* parseIntLiteral();
  DoubleLiteral* parseDoubleLiteral();
  StringLiteral* parseStringLiteral();

  Identifier* parseIdentifier();
  Type* parseType();

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
  Program* parseProgram();

};

#endif
