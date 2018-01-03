#include "Parser.h"
#include "Token.h"
#include "Tree.h"

#include <assert.h>

Parser::Parser(std::string path, ErrorReporter &err) : lexer{path, err}, error{err} {
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
    if (expect) error.report(tok, std::string("error: expected ") + str);
    return false;
  }
}

IntegerLiteral* Parser::parseIntegerLiteral() {
  Token tok = token();
  if (tok.is(Token::number)) {
    consume();
    return new IntegerLiteral(tok.lexeme);
  } else {
    return nullptr;
  }
}

Operator* Parser::parseOperator() {
  Token tok = token();
  if (tok.is(Token::operator_identifier)) {
    consume();
    return new Operator(tok.lexeme);
  } else {
    return nullptr;
  }
}

Identifier* Parser::parseIdentifier() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return new Identifier(tok.lexeme);
  } else {
    return nullptr;
  }
}

Expr* Parser::parseBasicValue() {
  if (token().is(Token::identifier)) {
    return parseIdentifier();
  } else if (token().is(Token::number)) {
    return parseIntegerLiteral();
  } else return nullptr;
}

Expr* Parser::parseBinaryExpr() {
    Expr* lhs = parseBasicValue();
    if (!lhs) {
      error.report(token(), "error: expected value");
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
  Identifier* type = parseIdentifier();
  if (!type) return nullptr;
  if (!parseTerminal(Token::semi, ";")) return nullptr;
  return new VarDecl(id, type);
}

VarDecl*  Parser::parseParamDecl() {
  Identifier* id = parseIdentifier();
  if (!id) return nullptr;
  if (!parseTerminal(Token::colon, ":")) return nullptr;
  Identifier* type = parseIdentifier();
  if (!type) return nullptr;
  return new VarDecl(id, type);
}

StmtList* Parser::parseParamList() {
  VarDecl* stmt = parseParamDecl();
  if (!stmt) return nullptr;
  if (!parseTerminal(Token::comma, ",", false)) return new StmtList(stmt, nullptr);
  StmtList* next = parseParamList();
  if (next == nullptr) error.report(token(), "error: extreneous comma");
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
  Identifier* ret = parseIdentifier();
  if (!ret) return nullptr;
  BlockStmt* block = parseBlockStmt();
  if (!block) return nullptr;
  return new FuncDecl(id, params, ret, block);
}

Stmt* Parser::parseStmt() {
    if (token().is(Token::key_var)) return parseVarDecl();
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
