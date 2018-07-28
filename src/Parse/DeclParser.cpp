#include "Parse/Parser.h"
#include "AST/Decl.h"
#include "AST/Type.h"
#include "Basic/CompilerException.h"

#include <memory>


std::shared_ptr<FuncDecl> Parser::parseUndefFuncDecl() {
  expectToken(Token::kw_func, "func");
  auto name = expectToken({Token::identifier, Token::operator_id}, "identifier");
  expectToken(Token::l_paren, "left parenthesis");
  auto param = acceptToken(Token::r_paren) ? std::vector<std::shared_ptr<ParamDecl>>() : parseParamDeclList();
  expectToken(Token::r_paren, "right parenthesis");  if (!consumeOperator("->")) throw CompilerException(token().getLocation(),  "error: expected ->");
  auto type = parseType();
  return std::make_shared<FuncDecl>(name, std::move(param), type, nullptr);
}

std::shared_ptr<Decl> Parser::parseDecl() {
  switch(token().getType()) {
  case Token::kw_var: return parseVarDecl();
  case Token::kw_let: return parseLetDecl();
  case Token::kw_func: return parseFuncDecl();
  case Token::kw_typealias: return parseTypeAlias();
  default: throw CompilerException(token().getLocation(),  "error: unable to parse decl");
  }
}
std::shared_ptr<TypeAlias> Parser::parseTypeAlias() {
  expectToken(Token::kw_typealias, "typedecl");
  auto name = expectToken(Token::identifier, "identifier");
  if (!consumeOperator("=")) throw CompilerException(token().getLocation(),  "expected '='");
  auto type = parseType();
  return std::make_shared<TypeAlias>(name, move(type));
}

std::shared_ptr<VarDecl> Parser::parseVarDecl() {
  expectToken(Token::kw_var, "var");
  auto name = expectToken(Token::identifier, "identifier");
  std::shared_ptr<Type> type = consumeToken(Token::colon)? parseType(): nullptr;
  scope.addType(name.lexeme, type);
  if (consumeOperator("=")) {
    auto expr = parseExpr();
    return std::make_shared<VarDecl>(name, move(type), move(expr));
  } else throw CompilerException(token().getLocation(),  "variables must be initialized at declaration");
}

std::shared_ptr<LetDecl> Parser::parseLetDecl() {
  expectToken(Token::kw_let, "let");
  auto name = expectToken(Token::identifier, "identifier");
  std::shared_ptr<Type> type = consumeToken(Token::colon)? parseType(): nullptr;
  scope.addType(name.lexeme, type);
  if (consumeOperator("=")) {
    auto expr = parseExpr();
    return std::make_shared<LetDecl>(name, move(type), move(expr));
  } else throw CompilerException(token().getLocation(),  "constants must be initialized at declaration");
}

std::shared_ptr<ParamDecl> Parser::parseParamDecl() {
  auto name = expectToken(Token::identifier, "identifier");
  expectToken(Token::colon, "colon");
  auto type = parseType();
  scope.addType(name.lexeme, type);
  return std::make_shared<ParamDecl>(name, type);
}

std::vector<std::shared_ptr<ParamDecl>> Parser::parseParamDeclList() {
  std::vector<std::shared_ptr<ParamDecl>> elements;
  elements.push_back(parseParamDecl());
  while (consumeToken(Token::comma)) {
    elements.push_back(parseParamDecl());
  }
  return elements;
}

std::shared_ptr<FuncDecl> Parser::parseFuncDecl() {
  // add function scoping
  scope.push();
  expectToken(Token::kw_func, "func");
  auto name = expectToken({Token::identifier, Token::operator_id}, "identifier");
  expectToken(Token::l_paren, "left parenthesis");
  auto param = acceptToken(Token::r_paren) ? std::vector<std::shared_ptr<ParamDecl>>() : parseParamDeclList();
  expectToken(Token::r_paren, "right parenthesis");
  if (!consumeOperator("->")) throw CompilerException(token().getLocation(),  "error: expected ->");
  auto type = parseType();
  // make function available for recursion
  scope.addType(name.lexeme, type);
  auto stmt = parseCompoundStmt();
  // remove function scoping
  scope.pop();
  // make function available to outer scope
  scope.addType(name.lexeme, type);
  return std::make_shared<FuncDecl>(name, std::move(param), type, stmt);
}
