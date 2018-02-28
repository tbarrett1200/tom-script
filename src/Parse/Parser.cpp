#include "Parse/Parser.h"
#include "Parse/Token.h"
#include "AST/Tree.h"
#include "Basic/ErrorReporter.h"
#include "Parse/Operator.h"

#include <assert.h>
#include <iostream>

Parser::Parser(SourceCode *src) : source{src}, lexer{src} {
  while(true) {
    Token t = lexer.next();
    if (t.is(Token::eof)) break;
    tokens.push_back(t);
  }
}

//=*****************************************************************************
//  # Utility
//=*****************************************************************************


Token Parser::token(int index) {
  if (index >= tokens.size()) {
    return lexer.getEOF();
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

void Parser::report(Token tok, std::string msg) {
  ErrorReporter{source}.report(tok, msg);
}

bool Parser::parseTerminal(int type, std::string str, bool expect = true) {
  Token tok = token();
  if (tok.is(type) && tok.lexeme == str) {
    consume();
    return true;
  } else {
    if (expect) {
      ErrorReporter{source}.report(tok, std::string("Error: expected ") + str);
    }
    return false;
  }
}

bool Parser::parseToken(int type) {
  Token tok = token();
  if (tok.is(type)) {
    consume();
    return true;
  } else {
    return false;
  }
}

/*

// ******************************************************************************
//  # Level 1
//  * program
// ******************************************************************************

// <stmt-list>? <eof>! **
Program* Parser::parseProgram() {
  StmtList* stmts = parseStmtList();
  if (token().isNot(Token::eof)) {
    ErrorReporter{source}.report(token(), "Error: expected end of file");
  }
  return new Program(stmts);
}

//=*****************************************************************************
//  # Level 2
// * stmt-list
//=*****************************************************************************

// <stmt> <stmt-list>? **
StmtList* Parser::parseStmtList() {
  Stmt* stmt = parseStmt();
  if (!stmt) return nullptr;
  StmtList* next = parseStmtList();
  return new StmtList(stmt, next);
}


//=*****************************************************************************
//  # Level 3
//  * stmt
//  * var-decl
//  * block-stmt
// * func-decl
//  * if-stmt
//  * while-stmt
//  * return-stmt
//  * expr-stmt
//=*****************************************************************************

//
// * <var-decl>       'var'
// * <block-stmt>     '{'
// * <func-decl>      'func'
// * <if-stmt>        'if'
// * <while-stmt>     'while'
// * <return-stmt>    'return'
// * <expr-stmt>      { IDENTIFIER, NUMBER, STRING_LITERAL, '(' }
// **
Stmt* Parser::parseStmt() {
    if (token().is(Token::key_var)) return parseVarDecl();
    else if (token().is(Token::l_brace)) return parseBlockStmt();
    else if (token().is(Token::key_func)) return parseFuncDecl();
    else if (token().is(Token::key_if)) return parseIfStmt();
    else if (token().is(Token::key_while)) return parseWhileStmt();
    else if (token().is(Token::key_return)) return parseReturnStmt();
    else if (token().isAny({
    Token::identifier,
    Token::number,
    Token::string_literal,
    Token::l_paren})) {
      return parseExprStmt();
    }
    else return nullptr;
}

// 'var' <identifier> ':' <type>  ';' **
VarDecl* Parser::parseVarDecl() {
  Identifier* id;
  TypeNode* type = nullptr;
  Expr* value = nullptr;

  if (!parseTerminal(Token::key_var, "var")) return nullptr;

  if (!(id = parseIdentifier())) {
    ErrorReporter{source}.report(token(), "Error: expected identifier");
    return nullptr;
  }

  if (parseTerminal(Token::colon, ":", false)) {
    if (!(type=parseTypeNode())) {
      ErrorReporter{source}.report(token(), "Error: expected type");
      return nullptr;
    }
  }

  if (parseTerminal(Token::operator_identifier, "=", false)) {
    if (!(value=parseExpr())) {
      ErrorReporter{source}.report(token(), "Error: var decl: expected expression");
      return nullptr;
    }
  }
  if (!parseTerminal(Token::semi, ";")) return nullptr;

  return new VarDecl(id, type, value);
}

// '{' <statement-list>? '}' **
BlockStmt* Parser::parseBlockStmt() {
  if (!parseTerminal(Token::l_brace, "{")) return nullptr;
  StmtList* stmts = parseStmtList();
  if (!parseTerminal(Token::r_brace, "}")) return nullptr;
  return new BlockStmt(stmts);
}

// 'func' <identifier> '(' <param-list> ')' '->' <type> <block-stmt> **
FuncDecl* Parser::parseFuncDecl() {
  if (!parseTerminal(Token::key_func, "func")) return nullptr;
  Identifier* id = parseIdentifier();
  if (!id) {
    ErrorReporter{source}.report(token(), "Error: expected identifier");
    return nullptr;
  }
  if (!parseTerminal(Token::l_paren, "(")) return nullptr;
  StmtList* params = parseParamList();
  if (!parseTerminal(Token::r_paren, ")")) return nullptr;
  if (!parseTerminal(Token::operator_identifier, "->")) return nullptr;
  TypeNode* ret = parseTypeNode();
  if (!ret) {
    ErrorReporter{source}.report(token(), "Error: expected type");
    return nullptr;
  }
  BlockStmt* block = parseBlockStmt();
  if (!block) {
    ErrorReporter{source}.report(token(), "Error: expected block");
    return nullptr;
  }
  return new FuncDecl(id, params, ret, block);
}

// 'if' '(' <expr> ')' <block-stmt> **
IfStmt* Parser::parseIfStmt() {
  if (!parseTerminal(Token::key_if, "if")) return nullptr;
  if (!parseTerminal(Token::l_paren, "(")) return nullptr;
  Expr* cond = parseExpr();
  if (!cond) {
    ErrorReporter{source}.report(token(), "Error: expected conditional");
    return nullptr;
  }
  if (!parseTerminal(Token::r_paren, ")")) return nullptr;
  BlockStmt* block = parseBlockStmt();
  if (!block) {
    ErrorReporter{source}.report(token(), "Error: expected block");
    return nullptr;
  }
  return new IfStmt(cond, block);
}

// 'while' '(' <expr> ')' <block-stmt> **
WhileStmt* Parser::parseWhileStmt() {
  if (!parseTerminal(Token::key_while, "while")) return nullptr;
  if (!parseTerminal(Token::l_paren, "(")) return nullptr;
  Expr* cond = parseExpr();
  if (!cond) {
    ErrorReporter{source}.report(token(), "Error: expected conditional");
    return nullptr;
  }
  if (!parseTerminal(Token::r_paren, ")")) return nullptr;
  BlockStmt* block = parseBlockStmt();
  if (!block) {
    ErrorReporter{source}.report(token(), "Error: expected block");
    return nullptr;
  }
  return new WhileStmt(cond, block);
}

// 'return' <expr>? ';' **
ReturnStmt* Parser::parseReturnStmt() {
  if (!parseTerminal(Token::key_return, "return")) return nullptr;
  Expr* expr = parseExpr();
  if (!parseTerminal(Token::semi, ";")) return nullptr;
  return new ReturnStmt(expr);
}

// <expr> ';' **
ExprStmt* Parser::parseExprStmt() {
  Expr* expr = parseExpr();
  if (!expr) {
    return nullptr;
  }
  if (!parseTerminal(Token::semi, ";")) return nullptr;
  return new ExprStmt(expr);
}

*/
