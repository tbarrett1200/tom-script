#include "Parse/Parser.h"
#include "AST/Decl.h"
#include "AST/Type.h"

#include <memory>

unique_ptr<Decl> Parser::parseDecl() {
  switch(token().getType()) {
  case Token::kw_var: return parseVarDecl();
  case Token::kw_let: return parseLetDecl();
  case Token::kw_func: return parseFuncDecl();
  case Token::kw_typealias: return parseTypeAlias();
  default:
    report(token(), "error: unable to parse decl");
    return nullptr;
  }
}
unique_ptr<TypeAlias> Parser::parseTypeAlias() {
  if (!consumeToken(Token::kw_typealias)) {
    report(token(), "expected typedecl");
    return nullptr;
  }
  auto name = token();
  if (!consumeToken(Token::identifier)) {
    report(name, "expected identifier");
    return nullptr;
  }
  if (!consumeOperator("=")) {
    report(token(), "expected '='");
    return nullptr;
  }
  auto type = parseType();
  if (!type) {
    report(token(), "expected type");
    return nullptr;
  }
  return make_unique<TypeAlias>(name, move(type));
}

unique_ptr<VarDecl> Parser::parseVarDecl() {
  if (!consumeToken(Token::kw_var)) {
    report(token(), "expected var");
    return nullptr;
  }
  auto name = token();
  if (!consumeToken(Token::identifier)) {
    report(name, "expected identifier");
    return nullptr;
  }
  unique_ptr<Type> type;
  if (consumeToken(Token::colon)) {
    type = parseType();
    if (!type) {
      report(token(), "expected type");
      return nullptr;
    }
  }
  unique_ptr<Expr> expr;
  if (consumeOperator("=")){
    expr = parseExpr();
    if (!expr) {
      report(token(), "expected expr");
      return nullptr;
    }
  }
  if (!type && !expr) {
    report(token(), "expected type or expression");
    return nullptr;
  } else {
    return make_unique<VarDecl>(name, move(type), move(expr));
  }
}

unique_ptr<LetDecl> Parser::parseLetDecl() {
  if (!consumeToken(Token::kw_let)) {
    report(token(), "expected let");
    return nullptr;
  }
  auto name = token();
  if (!consumeToken(Token::identifier)) {
    report(name, "expected identifier");
    return nullptr;
  }
  unique_ptr<Type> type;
  if (consumeToken(Token::colon)) {
    type = parseType();
    if (!type) return nullptr;
  }
  if (consumeOperator("=")){
    auto expr = parseExpr();
    if (!expr) return nullptr;
    return make_unique<LetDecl>(name, move(type), move(expr));
  } else {
    report(token(), "constants must be initialized at declaration");
    return nullptr;
  }
}

unique_ptr<FuncDecl> Parser::parseFuncDecl() {
  if (!consumeToken(Token::kw_func)) {
    report(token(), "expected func");
    return nullptr;
  }
  auto name = token();
  if (!consumeToken(Token::identifier)) {
    report(name, "expected identifier");
    return nullptr;
  }
  auto type = parseFunctionType(true);
  if (!type) {
    report(token(), "expected type");
    return nullptr;
  }
  return make_unique<FuncDecl>(name, move(type));
}
