#include "Parser.h"
#include "Token.h"
#include "Tree.h"
#include "ErrorReporter.h"

#include <assert.h>

Parser::Parser(SourceCode *src) : source{src}, lexer{src} {
  while(true) {
    Token t = lexer.next();
    if (t.is(Token::eof)) break;
    tokens.push_back(t);
  }
}

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

Expr* Parser::parseExpr() {
  return parseBinaryExpr();

}

bool Parser::parseTerminal(int type, std::string str, bool expect = true) {
  Token tok = token();
  if (tok.is(type) && tok.lexeme == str) {
    consume();
    return true;
  } else {
    if (expect) ErrorReporter{source}.report(tok, std::string("Error: expected ") + str);
    return false;
  }
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


Operator* Parser::parseOperator() {
  Token tok = token();
  if (tok.is(Token::operator_identifier)) {
    consume();
    return new Operator(tok);
  } else {
    return nullptr;
  }
}

Identifier* Parser::parseIdentifier() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return new Identifier(tok);
  } else {
    return nullptr;
  }
}

Type* Parser::parseType() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return new Type(tok);
  } else {
    return nullptr;
  }
}

Expr* Parser::parseValueExpr() {
  if (token().is(Token::identifier)) {
    return parseIdentifier();
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

Expr* Parser::parseBinaryExpr() {
    Expr* lhs = parseValueExpr();
    if (!lhs) {
      ErrorReporter{source}.report(token(), "Error: expected value");
      return nullptr;
    }
    if (token().is(Token::operator_identifier)) {
      Operator* op = parseOperator();
      Expr* rhs = parseBinaryExpr();
      return new BinaryExpr(lhs, op, rhs);
    } else return lhs;
}

VarDecl* Parser::parseVarDecl() {
  if (!parseTerminal(Token::key_var, "var")) return nullptr;
  Identifier* id = parseIdentifier();
  if (!id) return nullptr;
  if (!parseTerminal(Token::colon, ":")) return nullptr;
  Type* type = parseType();
  if (!type) return nullptr;
  if (!parseTerminal(Token::semi, ";")) return nullptr;
  return new VarDecl(id, type);
}

VarDecl*  Parser::parseParamDecl() {
  Identifier* id = parseIdentifier();
  if (!id) return nullptr;
  if (!parseTerminal(Token::colon, ":")) return nullptr;
  Type* type = parseType();
  if (!type) return nullptr;
  return new VarDecl(id, type);
}

StmtList* Parser::parseParamList() {
  VarDecl* stmt = parseParamDecl();
  if (!stmt) return nullptr;
  if (!parseTerminal(Token::comma, ",", false)) return new StmtList(stmt, nullptr);
  StmtList* next = parseParamList();
  if (next == nullptr) ErrorReporter{source}.report(token(), "Error: extreneous comma");
  return new StmtList(stmt, next);
}

FuncDecl* Parser::parseFuncDecl() {
  if (!parseTerminal(Token::key_func, "func")) return nullptr;
  Identifier* id = parseIdentifier();
  if (!id) return nullptr;
  if (!parseTerminal(Token::l_paren, "(")) return nullptr;
  StmtList* params = parseParamList();
  if (!parseTerminal(Token::r_paren, ")")) return nullptr;
  if (!parseTerminal(Token::operator_identifier, "->")) return nullptr;
  Type* ret = parseType();
  if (!ret) return nullptr;
  BlockStmt* block = parseBlockStmt();
  if (!block) return nullptr;
  return new FuncDecl(id, params, ret, block);
}

Stmt* Parser::parseStmt() {
    if (token().is(Token::key_var)) return parseVarDecl();
    else if (token().is(Token::l_brace)) return parseBlockStmt();
    else if (token().is(Token::key_func)) return parseFuncDecl();
    else if (token().is(Token::key_if)) return parseIfStmt();
    else if (token().is(Token::key_while)) return parseWhileStmt();
    else if (token().is(Token::key_return)) return parseReturnStmt();
    else if (token().isAny({Token::number, Token::identifier})) return parseExprStmt();
    else return nullptr;
}

StmtList* Parser::parseStmtList() {
  Stmt* stmt = parseStmt();
  if (!stmt) return nullptr;
  StmtList* next = parseStmtList();
  return new StmtList(stmt, next);
}

ReturnStmt* Parser::parseReturnStmt() {
  if (!parseTerminal(Token::key_return, "return")) return nullptr;
  Expr* expr = parseExpr();
  if (!expr) return nullptr;
  if (!parseTerminal(Token::semi, ";")) return nullptr;
  return new ReturnStmt(expr);
}

BlockStmt* Parser::parseBlockStmt() {
  if (!parseTerminal(Token::l_brace, "{")) return nullptr;
  StmtList* stmts = parseStmtList();
  if (!stmts) return nullptr;
  if (!parseTerminal(Token::r_brace, "}")) return nullptr;
  return new BlockStmt(stmts);
}

IfStmt* Parser::parseIfStmt() {
  if (!parseTerminal(Token::key_if, "if")) return nullptr;
  if (!parseTerminal(Token::l_paren, "(")) return nullptr;
  Expr* cond = parseExpr();
  if (!cond) return nullptr;
  if (!parseTerminal(Token::r_paren, ")")) return nullptr;
  BlockStmt* block = parseBlockStmt();
  if (!block) return nullptr;
  return new IfStmt(cond, block);
}

ExprStmt* Parser::parseExprStmt() {
  Expr* expr = parseExpr();
  if (!expr) return nullptr;
  if (!parseTerminal(Token::semi, ";")) return nullptr;
  return new ExprStmt(expr);
}

WhileStmt* Parser::parseWhileStmt() {
  if (!parseTerminal(Token::key_while, "while")) return nullptr;
  if (!parseTerminal(Token::l_paren, "(")) return nullptr;
  Expr* cond = parseExpr();
  if (!cond) return nullptr;
  if (!parseTerminal(Token::r_paren, ")")) return nullptr;
  BlockStmt* block = parseBlockStmt();
  if (!block) return nullptr;
  return new WhileStmt(cond, block);
}

Program* Parser::parseProgram() {
  StmtList* stmts = parseStmtList();
  if (!token().is(Token::eof)) {
    ErrorReporter{source}.report(0, 0, "Error: expected end of file");
  }
  return new Program(stmts);
}
