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
  default: throw report(token(), "error: unable to parse decl");
  }
}
unique_ptr<TypeAlias> Parser::parseTypeAlias() {
  expectToken(Token::kw_typealias, "typedecl");
  auto name = expectToken(Token::identifier, "identifier");
  if (!consumeOperator("=")) throw report(token(), "expected '='");
  auto type = parseType();
  return make_unique<TypeAlias>(name, move(type));
}

unique_ptr<VarDecl> Parser::parseVarDecl() {
  expectToken(Token::kw_var, "var");
  auto name = expectToken(Token::identifier, "identifier");
  unique_ptr<Type> type = consumeToken(Token::colon)? parseType(): nullptr;
  unique_ptr<Expr> expr = consumeOperator("=")? parseExpr(): nullptr;
  if (type || expr) return make_unique<VarDecl>(name, move(type), move(expr));
  else throw report(token(), "expected type or expression");
}

unique_ptr<LetDecl> Parser::parseLetDecl() {
  expectToken(Token::kw_let, "let");
  auto name = expectToken(Token::identifier, "identifier");
  unique_ptr<Type> type = consumeToken(Token::colon)? parseType(): nullptr;
  if (consumeOperator("=")) {
    auto expr = parseExpr();
    return make_unique<LetDecl>(name, move(type), move(expr));
  } else throw report(token(), "constants must be initialized at declaration");
}

unique_ptr<FuncDecl> Parser::parseFuncDecl() {
  expectToken(Token::kw_func, "func");
  auto name = expectToken(Token::identifier, "identifier");
  auto type = parseFunctionType(true);
  return make_unique<FuncDecl>(name, move(type));
}
