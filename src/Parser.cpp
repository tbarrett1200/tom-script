#include "Parser.h"
#include "Token.h"
#include "Tree.h"

#include <assert.h>

Parser::Parser(std::string path) : lexer{path} {
  while(true) {
    Token t = lexer.next();
    if (t.is(Token::eof)) break;
    tokens.push_back(t);
  }

  for (auto tok: tokens)
    std::cout << tok << std::endl;
}

Token Parser::token(int index) {
  if (index >= tokens.size()) {
    return Token("<eof>", Token::eof, 0, 0, 0);
  }
  return tokens.at(index);
}

auto Parser::consume() -> void {
  if (tokens.size() > 0) {
    return tokens.pop_front();
  }
}

ParseTree Parser::parseExpr() {
  return {ParseTree::Expr, {parseBinaryExpr()}};
}

ParseTree Parser::parseIntegerLiteral() {
  Token tok = token();
  if (tok.is(Token::number)) {
    consume();
    return {ParseTree::IntegerLiteral, tok};
  }

  assert(false && "error: parse integer literal");
}

ParseTree Parser::parseOperator() {
  Token tok = token();
  if (tok.is(Token::operator_identifier)) {
    consume();
    return {ParseTree::Operator, tok};
  }

  assert(false && "error: parse operator");
}

ParseTree Parser::parseIdentifier() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return {ParseTree::Identifier, tok};
  }

  assert(false && "error: parse identifier");
}

ParseTree Parser::parseBinaryExpr() {
  ParseTree lhs;

  if (token().is(Token::identifier)) {
    lhs = parseIdentifier();
  } else if (token().is(Token::number)) {
    lhs = parseIntegerLiteral();
  } else {
    assert(false && "error: parse binary operator");
  }

  if (token().is(Token::operator_identifier)) {
    ParseTree op = parseOperator();
    ParseTree rhs = parseBinaryExpr();
    return {ParseTree::BinaryExpr, {lhs, op, rhs}};
  } else {
    return lhs;
  }
}

ParseTree Parser::parseDecl() {
  if (token().is(Token::key_var)) {
    return {ParseTree::Decl,{parseVarDecl()}};
  } else if (token().is(Token::key_let)) {
    return {ParseTree::Decl,{parseConstDecl()}};
  } else if (token().is(Token::key_func)) {
    return {ParseTree::Decl,{parseFuncDecl()}};
  } else {
    assert(false && "error: parse decl");
  }
}

ParseTree Parser::parseVarDecl() {
  ParseTree key = {ParseTree::Terminal, {token()}};
  consume();
  ParseTree id = {ParseTree::Terminal, {token()}};
  consume();
  ParseTree col = {ParseTree::Terminal, {token()}};
  consume();
  ParseTree type = {ParseTree::Terminal, {token()}};
  consume();
  return {ParseTree::VarDecl, {key, id, col, type}};
}

ParseTree Parser::parseConstDecl() {
  ParseTree key = {ParseTree::Terminal, {token()}};
  consume();
  ParseTree id = {ParseTree::Identifier, {token()}};
  consume();
  ParseTree col = {ParseTree::Terminal, {token()}};
  consume();
  ParseTree type = {ParseTree::Identifier, {token()}};
  consume();
  return {ParseTree::ConstDecl, {key, id, col, type}};
}

ParseTree Parser::parseParamDecl() {
  ParseTree id = {ParseTree::Terminal, {token()}};
  consume();
  ParseTree col = {ParseTree::Terminal, {token()}};
  consume();
  ParseTree type = {ParseTree::Terminal, {token()}};
  consume();
  return {ParseTree::ParamDecl, {id, col, type}};
}


ParseTree Parser::parseParamDeclList() {
  std::vector<ParseTree> decls;

  if (token().isNot(Token::r_brace)) {
    decls.push_back(parseParamDecl());
    while (token().is(Token::comma)) {
      decls.push_back({ParseTree::Terminal, token()});
      consume();
      decls.push_back(parseParamDecl());
    }
  }
  return {ParseTree::ParamDeclList, decls};
}

ParseTree Parser::parseFuncDecl() {
  ParseTree func = {ParseTree::Terminal, token()};
  consume();

  ParseTree id = {ParseTree::Identifier, token()};
  consume();

  ParseTree lp = {ParseTree::Terminal, token()};
  consume();

  ParseTree param = parseParamDeclList();

  ParseTree rp = {ParseTree::Terminal, token()};
  consume();

  ParseTree arrow = {ParseTree::Terminal, token()};
  consume();

  ParseTree retType = {ParseTree::Identifier, token()};
  consume();

  ParseTree block = parseBlockStmt();

  return {ParseTree::FuncDecl, {func, id, lp, param, rp, arrow, retType, block}};
}

ParseTree Parser::parseStmt() {
  if (token().isAny({Token::key_var, Token::key_let})) {
    return {ParseTree::Stmt,{parseDeclStmt()}};
  } else if (token().is(Token::key_func)) {
    return {ParseTree::Stmt,{parseFuncDecl()}};
  } else if (token().is(Token::key_if)) {
    return {ParseTree::Stmt,{parseIfStmt()}};
  } else if (token().is(Token::key_while)) {
    return {ParseTree::Stmt,{parseWhileStmt()}};
  } else if (token().is(Token::key_return)) {
    return {ParseTree::Stmt,{parseReturnStmt()}};
  } else {
    return {ParseTree::Stmt,{parseExprStmt()}};
  }
}

ParseTree Parser::parseReturnStmt() {
  ParseTree key = {ParseTree::Terminal, token()};
  consume();

  ParseTree expr = parseExpr();

  ParseTree semi = {ParseTree::Terminal, token()};
  consume();

  return {ParseTree::ReturnStmt, {key, expr, semi}};
}


ParseTree Parser::parseStmtList() {
  std::vector<ParseTree> stmts;
  while (token().isNot(Token::eof) && token().isNot(Token::r_brace)) {
    stmts.push_back(parseStmt());
  }
  return {ParseTree::StmtList, stmts};
}

ParseTree Parser::parseBlockStmt() {
  ParseTree lb = {ParseTree::Terminal, token()};
  consume();

  ParseTree decl = parseStmtList();

  if (token().isNot(Token::r_brace)) {
    assert(false && "error: missing r_brace");
  }

  ParseTree rb = {ParseTree::Terminal, token()};
  consume();

  return {ParseTree::BlockStmt, {lb, decl, rb}};
}


ParseTree Parser::parseDeclStmt() {
  ParseTree decl = parseDecl();
  Token tok = token();
  if (tok.is(Token::semi)) {
    consume();
    return {ParseTree::DeclStmt, {decl, {ParseTree::Terminal,tok}}};
  }

  assert(false && "error: parse decl-stmt");
}

ParseTree Parser::parseIfStmt() {
  ParseTree key = {ParseTree::Terminal, token()};
  consume();

  ParseTree lp = {ParseTree::Terminal, token()};
  consume();

  ParseTree expr = parseExpr();

  ParseTree rp = {ParseTree::Terminal, token()};
  consume();

  ParseTree block = parseBlockStmt();

  return {ParseTree::IfStmt, {key, lp, expr, rp, block}};
}

ParseTree Parser::parseWhileStmt() {
  ParseTree key = {ParseTree::Terminal, token()};
  consume();

  ParseTree lp = {ParseTree::Terminal, token()};
  consume();

  ParseTree expr = parseExpr();

  ParseTree rp = {ParseTree::Terminal, token()};
  consume();

  ParseTree block = parseBlockStmt();

  return {ParseTree::WhileStmt, {key, lp, expr, rp, block}};
}
ParseTree Parser::parseExprStmt() {
  ParseTree expr = parseExpr();
  Token tok = token();
  if (tok.is(Token::semi)) {
    consume();
    return {ParseTree::ExprStmt, {expr, {ParseTree::Terminal,tok}}};
  }

  assert(false && "error: parse expr-stmt");
}
