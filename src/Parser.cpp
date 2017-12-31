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
    return Token("<eof>", Token::eof, 0, 0, 0);
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

ParseTree Parser::parseExpr() {
  try {
    return {ParseTree::Expr, {parseBinaryExpr()}};
  } catch (ParseTree::Type err) {
    consumeUntil({Token::semi, Token::eof});
    return {ParseTree::Expr, Token{}};
  }
}

ParseTree Parser::parseIntegerLiteral() {
  Token tok = token();
  if (tok.is(Token::number)) {
    consume();
    return {ParseTree::IntegerLiteral, tok};
  } else {
    error.report(token(), "error: expected integer literal");
    throw ParseTree::IntegerLiteral;
  }
}

ParseTree Parser::parseOperator() {
  Token tok = token();
  if (tok.is(Token::operator_identifier)) {
    consume();
    return {ParseTree::Operator, tok};
  } else {
    error.report(token(), "error: expected operator");
    throw ParseTree::Operator;
  }
}

ParseTree Parser::parseIdentifier() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return {ParseTree::Identifier, tok};
  } else {
    error.report(token(), "error: expected identifier");
    throw ParseTree::Identifier;
  }
}

ParseTree Parser::parseTerminal(int type, std::string str) {
  Token tok = token();
  if (tok.is(type) && tok.lexeme == str) {
    consume();
    return {ParseTree::Terminal, tok};
  } else {
    error.report(tok, std::string("error: expected ") + str);
    throw ParseTree::Terminal;
  }
}

ParseTree Parser::parseBasicValue() {
  if (token().is(Token::identifier)) {
    return parseIdentifier();
  } else if (token().is(Token::number)) {
    return parseIntegerLiteral();
  } else {
    error.report(token(), "error: expected value");
    throw ParseTree::BasicValue;
  }
}

ParseTree Parser::parseBinaryExpr() {

  try {

    ParseTree lhs = parseBasicValue();
    if (token().is(Token::operator_identifier)) {
      ParseTree op = parseOperator();
      ParseTree rhs = parseBinaryExpr();
      return {ParseTree::BinaryExpr, {lhs, op, rhs}};
    } else return lhs;

  } catch (ParseTree::Type err) {
    throw ParseTree::BinaryExpr;
  }
}

ParseTree Parser::parseDecl() {
  try {
    if (token().is(Token::key_var)) {
      return parseVarDecl();
    } else if (token().is(Token::key_let)) {
      return parseConstDecl();
    } else if (token().is(Token::key_func)) {
      return parseFuncDecl();
    } else {
      error.report(token(), "error: expected declaration");
      throw ParseTree::Decl;
    }
  } catch (ParseTree::Type err) {
    throw ParseTree::Decl;
  }
}

ParseTree Parser::parseVarDecl() {
  try {
    ParseTree key = parseTerminal(Token::key_var, "var");
    ParseTree id = parseIdentifier();
    ParseTree col = parseTerminal(Token::colon, ":");
    ParseTree type = parseIdentifier();
    return {ParseTree::VarDecl, {key, id, col, type}};
  } catch (ParseTree::Type) {
    error.report(token(), "error: expected variable declation");
    throw ParseTree::VarDecl;
  }
}

ParseTree Parser::parseConstDecl() {
  try {
    ParseTree key = parseTerminal(Token::key_let, "let");
    ParseTree id = parseIdentifier();
    ParseTree col = parseTerminal(Token::colon, ":");
    ParseTree type = parseIdentifier();
    return {ParseTree::ConstDecl, {key, id, col, type}};
  } catch (ParseTree::Type) {
    error.report(token(), "error: expected constant variable declation");
    throw ParseTree::ConstDecl;
  }
}

ParseTree Parser::parseParamDecl() {
  try {
    ParseTree id = parseIdentifier();
    ParseTree col = parseTerminal(Token::colon, ":");
    ParseTree type = parseIdentifier();
    return {ParseTree::ParamDecl, {id, col, type}};
  } catch (ParseTree::Type) {
    error.report(token(), "error: expected parameter declaration");
    throw ParseTree::ParamDecl;
  }
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
  try {
    ParseTree func = parseTerminal(Token::key_func, "func");

    ParseTree id;

    try {
      id = parseIdentifier();
    } catch(ParseTree::Type err) {
      if (token().isNot(Token::l_paren)) throw err;
      id = ParseTree{ParseTree::Identifier, Token()};
    }

    ParseTree lp = parseTerminal(Token::l_paren, "(");
    ParseTree param = parseParamDeclList();
    ParseTree rp = parseTerminal(Token::r_paren, ")");
    ParseTree arrow = parseTerminal(Token::operator_identifier, "->");
    ParseTree retType = parseIdentifier();
    ParseTree block = parseBlockStmt();
    return {ParseTree::FuncDecl, {func, id, lp, param, rp, arrow, retType, block}};
  } catch (ParseTree::Type) {
    throw ParseTree::FuncDecl;
  }
}

ParseTree Parser::parseStmt() {
  try {
    if (token().isAny({Token::key_var, Token::key_let})) {
      return parseDeclStmt();
    } else if (token().is(Token::key_func)) {
      return parseFuncDecl();
    } else if (token().is(Token::key_if)) {
      return parseIfStmt();
    } else if (token().is(Token::key_while)) {
      return parseWhileStmt();
    } else if (token().is(Token::key_return)) {
      return parseReturnStmt();
    } else if (token().isAny({Token::number, Token::identifier})){
      return parseExprStmt();
    } else {
      error.report(token(), "error: expected statement");
      throw ParseTree::Stmt;
    }
  } catch (ParseTree::Type) {
    throw ParseTree::Stmt;
  }
}

ParseTree Parser::parseStmtList() {
  std::vector<ParseTree> stmts;
  while (token().isNot(Token::eof) && token().isNot(Token::r_brace)) {
    stmts.push_back(parseStmt());
  }
  return {ParseTree::StmtList, stmts};
}

ParseTree Parser::parseReturnStmt() {
  try {
    ParseTree key = parseTerminal(Token::key_return, "return");
    ParseTree expr = parseExprStmt();
    return {ParseTree::ReturnStmt, {key, expr}};
  } catch (ParseTree::Type) {
    error.report(token(), "error: expected return statement");
    throw ParseTree::ReturnStmt;
  }
}

ParseTree Parser::parseBlockStmt() {
  try {
    ParseTree lb = parseTerminal(Token::l_brace, "{");
    ParseTree decl = parseStmtList();
    ParseTree rb = parseTerminal(Token::r_brace, "}");
    return {ParseTree::BlockStmt, {lb, decl, rb}};
  } catch (ParseTree::Type) {
    error.report(token(), "error: expected block statement");
    throw ParseTree::BlockStmt;
  }
}

ParseTree Parser::parseDeclStmt() {
  try {
    ParseTree decl = parseDecl();
    ParseTree semi = parseTerminal(Token::semi, ";");
    return {ParseTree::DeclStmt, {decl, semi}};
  } catch (ParseTree::Type err) {
    consumeUntil({Token::eof});
    return {ParseTree::DeclStmt, Token()};
  }
}

ParseTree Parser::parseIfStmt() {
  try {
    ParseTree key = parseTerminal(Token::key_if, "if");
    ParseTree lp = parseTerminal(Token::l_paren, "(");
    ParseTree expr = parseExpr();
    ParseTree rp = parseTerminal(Token::r_paren, ")");
    ParseTree block = parseBlockStmt();
    return {ParseTree::IfStmt, {key, lp, expr, rp, block}};
  } catch (ParseTree::Type) {
    error.report(token(), "error: expected if statement");
    throw ParseTree::IfStmt;
  }
}

ParseTree Parser::parseWhileStmt() {
  try {
    ParseTree key = parseTerminal(Token::key_while, "while");
    ParseTree lp = parseTerminal(Token::l_paren, "(");
    ParseTree expr = parseExpr();
    ParseTree rp = parseTerminal(Token::r_paren, ")");
    ParseTree block = parseBlockStmt();
    return {ParseTree::WhileStmt, {key, lp, expr, rp, block}};
  } catch (ParseTree::Type) {
    error.report(token(), "error: expected while statement");
    throw ParseTree::WhileStmt;
  }
}

ParseTree Parser::parseExprStmt() {
  try {
    ParseTree expr = parseExpr();
    ParseTree semi = parseTerminal(Token::semi, ";");
    return {ParseTree::ExprStmt, {expr, semi}};
  } catch (ParseTree::Type err) {
    if (err == ParseTree::Terminal) consumeUntil({Token::semi, Token::eof});
    consume();
    return {ParseTree::ExprStmt, Token()};
  }
}
