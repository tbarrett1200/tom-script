#include "Parse/Parser.h"
#include "AST/Decl.h"
#include "AST/TypeExpr.h"

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
  if (!parseToken(Token::kw_typealias)) {
    report(token(), "expected typedecl");
    return nullptr;
  }
  auto name = token();
  if (name.is(Token::identifier)) {
    consume();
  } else  {
    report(name, "expected identifier");
    return nullptr;
  }
  if (!parseTerminal(Token::operator_id, "=", false)) report(token(), "expected '='");
  auto type = parseType();
  if (!type) {
    report(token(), "expected type");
    return nullptr;
  }
  return make_unique<TypeAlias>(name, move(type));
}

unique_ptr<VarDecl> Parser::parseVarDecl() {
  if (!parseToken(Token::kw_var)) {
    report(token(), "expected var");
    return nullptr;
  }
  auto name = token();
  if (name.is(Token::identifier)) {
    consume();
  } else  {
    report(name, "expected identifier");
    return nullptr;
  }
  if (parseTerminal(Token::colon, ":", false)) {
    auto type = parseType();
    if (!type) {
      report(token(), "expected type");
      return nullptr;
    }
    return make_unique<VarDecl>(name, move(type));
  } else if (parseTerminal(Token::operator_id, "=", false)){
    auto expr = parseExpr();
    if (!expr) {
      report(token(), "expected expr");
      return nullptr;
    }
    return make_unique<VarDecl>(name, move(expr));
  } else {
    report(token(), "unable to parse var decl");
    return nullptr;
  }
}

unique_ptr<LetDecl> Parser::parseLetDecl() {
  if (!parseToken(Token::kw_let)) {
    report(token(), "expected let");
    return nullptr;
  }
  auto name = token();
  if (name.is(Token::identifier)) {
    consume();
  } else  {
    report(name, "expected identifier");
    return nullptr;
  }
  if (parseTerminal(Token::colon, ":", false)) {
    auto type = parseType();
    if (!type) {
      return nullptr;
    }
    if (!parseTerminal(Token::operator_id, "=", false)) {
      report(token(), "error: let decl must be initialized");
      return nullptr;
    }
    auto expr = parseExpr();
    if (!expr) {
      return nullptr;
    }
    return make_unique<LetDecl>(name, move(type), move(expr));
  } else if (parseTerminal(Token::operator_id, "=", false)){
    auto expr = parseExpr();
    if (!expr) {
      return nullptr;
    }
    return make_unique<LetDecl>(name, move(expr));
  } else {
    report(token(), "error: let decl must be initialized");
    return nullptr;
  }
}

unique_ptr<FuncDecl> Parser::parseFuncDecl() {
  if (!parseToken(Token::kw_func)) {
    report(token(), "expected func");
    return nullptr;
  }
  auto name = token();
  if (name.is(Token::identifier)) {
    consume();
  } else  {
    report(name, "expected identifier");
    return nullptr;
  }
  auto type = parseFunctionDeclType();
  if (!type) {
    report(token(), "expected type");
    return nullptr;
  }
  return make_unique<FuncDecl>(name, move(type));
}
