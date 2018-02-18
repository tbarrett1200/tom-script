#include "Parser.h"
#include "Token.h"
#include "Tree.h"
#include "ErrorReporter.h"
#include "Operator.h"

#include <assert.h>
#include <iostream>

Parser::Parser(SourceCode *src) : source{src}, lexer{src} {
  while(true) {
    Token t = lexer.next();
    if (t.is(Token::eof)) break;
    tokens.push_back(t);
  }
}

/*******************************************************************************
  # Utility
*******************************************************************************/


Token Parser::token(int index) {
  if (index >= tokens.size()) {
    return Token(std::string(1,char(-1)), Token::eof, 0, 0, 0);
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
      ErrorReporter{source}.report(tok, std::string("Error: expected ") + str);
    }
    return false;
  }
}

/*******************************************************************************
  # Level 1
  * program
*******************************************************************************/

/* <stmt-list>? <eof>! */
Program* Parser::parseProgram() {
  StmtList* stmts = parseStmtList();
  if (token().isNot(Token::eof)) {
    ErrorReporter{source}.report(token(), "Error: expected end of file");
  }
  return new Program(stmts);
}

/*******************************************************************************
  # Level 2
  * stmt-list
*******************************************************************************/

/* <stmt> <stmt-list>? */
StmtList* Parser::parseStmtList() {
  Stmt* stmt = parseStmt();
  if (!stmt) return nullptr;
  StmtList* next = parseStmtList();
  return new StmtList(stmt, next);
}


/*******************************************************************************
  # Level 3
  * stmt
  * var-decl
  * block-stmt
  * func-decl
  * if-stmt
  * while-stmt
  * return-stmt
  * expr-stmt
*******************************************************************************/

/*
 * <var-decl>       'var'
 * <block-stmt>     '{'
 * <func-decl>      'func'
 * <if-stmt>        'if'
 * <while-stmt>     'while'
 * <return-stmt>    'return'
 * <expr-stmt>      { IDENTIFIER, NUMBER, STRING_LITERAL, '(' }
 */
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

/* 'var' <identifier> ':' <type>  ';' */
VarDecl* Parser::parseVarDecl() {
  if (!parseTerminal(Token::key_var, "var")) return nullptr;
  Identifier* id = parseIdentifier();
  if (!id) {
    ErrorReporter{source}.report(token(), "Error: expected identifier");
    return nullptr;
  }
  if (!parseTerminal(Token::colon, ":")) return nullptr;
  Type* type = parseType();
  if (!type) {
    ErrorReporter{source}.report(token(), "Error: expected type");
    return nullptr;
  }
  if (parseTerminal(Token::operator_identifier, "=", false)) {
    Expr *expr = parseExpr();
    if (!expr) {
      ErrorReporter{source}.report(token(), "Error: expected expression");
      return nullptr;
    }
    if (!parseTerminal(Token::semi, ";")) return nullptr;
    return new VarDecl(id, type, expr);
  }
  if (!parseTerminal(Token::semi, ";")) return nullptr;
  return new VarDecl(id, type, nullptr);
}

/* '{' <statement-list>? '}' */
BlockStmt* Parser::parseBlockStmt() {
  if (!parseTerminal(Token::l_brace, "{")) return nullptr;
  StmtList* stmts = parseStmtList();
  if (!parseTerminal(Token::r_brace, "}")) return nullptr;
  return new BlockStmt(stmts);
}

/* 'func' <identifier> '(' <param-list> ')' '->' <type> <block-stmt> */
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
  Type* ret = parseType();
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

/* 'if' '(' <expr> ')' <block-stmt> */
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

/* 'while' '(' <expr> ')' <block-stmt> */
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

/* 'return' <expr>? ';' */
ReturnStmt* Parser::parseReturnStmt() {
  if (!parseTerminal(Token::key_return, "return")) return nullptr;
  Expr* expr = parseExpr();
  if (!parseTerminal(Token::semi, ";")) return nullptr;
  return new ReturnStmt(expr);
}

/* <expr> ';' */
ExprStmt* Parser::parseExprStmt() {
  Expr* expr = parseExpr();
  if (!expr) {
    return nullptr;
  }
  if (!parseTerminal(Token::semi, ";")) return nullptr;
  return new ExprStmt(expr);
}

/*******************************************************************************
  # Level 4
  * identifier
  * type
  * param-list
  * expr
*******************************************************************************/

/* IDENTIFER */
Identifier* Parser::parseIdentifier() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return new Identifier(tok);
  } else {
    return nullptr;
  }
}

/* IDENTIFIER */
Type* Parser::parseType() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return new Type(tok);
  } else {
    return nullptr;
  }
}

/* <param-decl> { ',' <param-list>! }? */
StmtList* Parser::parseParamList() {
  VarDecl* stmt = parseParamDecl();
  if (!stmt) return nullptr;
  if (parseTerminal(Token::comma, ",", false)) {
    StmtList* next = parseParamList();
    if (next == nullptr) {
      ErrorReporter{source}.report(token(), "Error: extreneous comma");
      return new StmtList(stmt, nullptr);;
    } else return new StmtList(stmt, next);
  } else return new StmtList(stmt, nullptr);
}


/* <binary-expr> */
Expr* Parser::parseExpr(int precedence) {
  switch(precedence) {
    case 0: return parseValueExpr();
    case 1: return parseUnaryExpr();
    default: return parseBinaryExpr(precedence);
  }
}

/*******************************************************************************
  # Level 5
  * param-decl
  * binary-expr
*******************************************************************************/

/* <identifier> ':' <type> */
VarDecl*  Parser::parseParamDecl() {
  Identifier* id = parseIdentifier();
  if (!id) return nullptr;
  if (!parseTerminal(Token::colon, ":")) return nullptr;
  Type* type = parseType();
  if (!type) return nullptr;
  return new VarDecl(id, type, nullptr);
}

Expr* Parser::parseUnaryExpr() {
  OperatorNode* op = parseOperator(1);
  Expr* expr = parseValueExpr();
  if (!expr) {
    ErrorReporter{source}.report(token(), "Error: expected expression");
    return nullptr;
  }
  if (!op) return expr;
  return new UnaryExpr(expr, op);
}
/* <value-expr> { <operator> <binary-expr>! } */
Expr* Parser::parseBinaryExpr(int precedence) {
  switch (OperatorTable::associativity(precedence)) {
    case Associativity::left:
      return parseInfixLeft(precedence);
    case Associativity::right:
      return parseInfixRight(precedence);
    case Associativity::none:
      return parseInfixNone(precedence);
  }
}

/*
 * <function-call>     IDENTIFIER       LOOKAHEAD('(')
 * <identifier>        IDENTIFIER       LOOKAHEAD(not '(')
 * <int-literal>       NUMBER           which isIntLiteral()
 * <double-literal>    NUMBER           which isDoubleLiteral()
 * <string-literal>    STRING_LITERAL
 * '(' <expr> ')'      '('
 */
Expr* Parser::parseValueExpr() {
  if (token().is(Token::identifier)) {
    if (token(1).is(Token::l_paren)) {
      return parseFunctionCall();
    }
    else return parseIdentifier();
  } else if (token().isIntLiteral()) {
    return parseIntLiteral();
  } else if (token().isDoubleLiteral()) {
    return parseDoubleLiteral();
  } else if (token().is(Token::string_literal)) {
    return parseStringLiteral();
  } else if (token().is(Token::l_paren)) {
    if (!parseTerminal(Token::l_paren, "(")) return nullptr;
    Expr* expr = parseExpr();
    if (!expr) return nullptr;
    if (!parseTerminal(Token::r_paren, ")")) return nullptr;
    return expr;
  } else return nullptr;
}

/*******************************************************************************
  # Level 6
  * value-expr
  * function-call
  * int-literal
  * double-literal
  * string-literal
*******************************************************************************/

/* <identifier> '(' <expr-list>? ')'*/
FunctionCall* Parser::parseFunctionCall() {
  Identifier* id = parseIdentifier();
  if (!id) return nullptr;
  if (!parseTerminal(Token::l_paren, "(")) return nullptr;
  ExprList* params = parseExprList();
  if (!parseTerminal(Token::r_paren, ")")) return nullptr;
  return new FunctionCall(id, params);
}

IntLiteral* Parser::parseIntLiteral() {
  Token tok = token();
  if (tok.is(Token::number) && tok.isIntLiteral()) {
    consume();
    return new IntLiteral(tok);
  } else {
    return nullptr;
  }
}

DoubleLiteral* Parser::parseDoubleLiteral() {
  Token tok = token();
  if (tok.is(Token::number) && tok.isDoubleLiteral()) {
    consume();
    return new DoubleLiteral(tok);
  } else {
    return nullptr;
  }
}

StringLiteral* Parser::parseStringLiteral() {
  Token tok = token();
  if (tok.is(Token::string_literal)) {
    consume();
    return new StringLiteral(tok);
  } else {
    return nullptr;
  }
}

Expr* Parser::parseInfixNone(int p) {
  Expr* left = parseExpr(p-1);
  if (!left) return nullptr;
  OperatorNode* op = parseOperator(p);
  if (!op) return left;
  Expr* right = parseExpr(p-1);
  if (!right) {
    ErrorReporter{source}.report(token(), "error: expected expression");
    return nullptr;
  }
  return new BinaryExpr(left, op, right);
}

Expr* Parser::parseInfixRight(int p) {
  Expr* left = parseExpr(p-1);
  if (!left) return nullptr;
  OperatorNode* op = parseOperator(p);
  if (!op) return left;
  Expr* right = parseExpr(p);
  if (!right) {
    ErrorReporter{source}.report(token(), "error: expected expression");
    return nullptr;
  }
  return new BinaryExpr(left, op, right);
}

Expr* Parser::parseInfixLeft(int precedence) {
  Expr* left = parseExpr(precedence-1);
  if (!left) return nullptr;
  std::function<Expr*(int,Expr*)> continueParse;
  continueParse = [this, &continueParse](int precedence, Expr* left) -> Expr* {
    OperatorNode* op = parseOperator(precedence);
    if (!op) return left;
    Expr* right = parseExpr(precedence-1);
    if (!right) {
      ErrorReporter{source}.report(token(), "error: expected expression");
      return nullptr;
    }
    return continueParse(precedence, new BinaryExpr(left,op,right));
  };
  return continueParse(precedence, left);
}


/******************************************************************************
  # Level 7
  * expr-list
  * operator
*******************************************************************************/

/* OPERATOR_IDENTIFIER */
OperatorNode* Parser::parseOperator(int precedence) {
  Token tok = token();
  if (OperatorTable::level(precedence).contains({tok.lexeme})) {
    consume();
    return new OperatorNode(tok);
  } else return nullptr;
}


/* <expr> { ',' <expr_list>?! } */
ExprList* Parser::parseExprList() {
  Expr* expr = parseExpr();
  if (!expr) return nullptr;
  if (parseTerminal(Token::comma, ",", false)) {
    ExprList *next = parseExprList();
    if (!next) {
      ErrorReporter{source}.report(token(), "Error: extreneous comma");
      return new ExprList(expr, nullptr);;
    } else return new ExprList(expr, next);
  } else return new ExprList(expr, nullptr);
}
