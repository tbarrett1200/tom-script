#include "Parse/Parser.h"
#include "AST/Decl.h"
#include "AST/Type.h"

#include <memory>


shared_ptr<Decl> Parser::makeDecl(std::string text) {
  const std::stringstream sstream{text};
  auto source = SourceCode{sstream, "factory"};
  auto parser = Parser{&source};
  auto type = parser.parseDecl();
  if (!type) throw std::string("parse error");
  return type;
}

shared_ptr<Decl> Parser::parseDecl() {
  switch(token().getType()) {
  case Token::kw_var: return parseVarDecl();
  case Token::kw_let: return parseLetDecl();
  case Token::kw_func: return parseFuncDecl();
  case Token::kw_typealias: return parseTypeAlias();
  default: throw report(token(), "error: unable to parse decl");
  }
}
shared_ptr<TypeAlias> Parser::parseTypeAlias() {
  expectToken(Token::kw_typealias, "typedecl");
  auto name = expectToken(Token::identifier, "identifier");
  if (!consumeOperator("=")) throw report(token(), "expected '='");
  auto type = parseType();
  return make_shared<TypeAlias>(name, move(type));
}

shared_ptr<VarDecl> Parser::parseVarDecl() {
  expectToken(Token::kw_var, "var");
  auto name = expectToken(Token::identifier, "identifier");
  shared_ptr<Type> type = consumeToken(Token::colon)? parseType(): nullptr;
  shared_ptr<Expr> expr = consumeOperator("=")? parseExpr(): nullptr;
  if (type || expr) return make_shared<VarDecl>(name, move(type), move(expr));
  else throw report(token(), "expected type or expression");
}

shared_ptr<LetDecl> Parser::parseLetDecl() {
  expectToken(Token::kw_let, "let");
  auto name = expectToken(Token::identifier, "identifier");
  shared_ptr<Type> type = consumeToken(Token::colon)? parseType(): nullptr;
  if (consumeOperator("=")) {
    auto expr = parseExpr();
    return make_shared<LetDecl>(name, move(type), move(expr));
  } else throw report(token(), "constants must be initialized at declaration");
}

shared_ptr<FuncDecl> Parser::parseFuncDecl() {
  expectToken(Token::kw_func, "func");
  auto name = expectToken({Token::identifier, Token::operator_id}, "identifier");
  auto type = parseFunctionType(true);
  return make_shared<FuncDecl>(name, move(type));
}
