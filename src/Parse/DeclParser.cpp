#include <memory>

#include "Basic/CompilerException.h"
#include "AST/Decl.h"
#include "AST/Type.h"
#include "Parse/Parser.h"
//
// std::unique_ptr<FuncDecl> Parser::parseUndefFuncDecl() {
//   expectToken(Token::kw_func, "func");
//   Token name = expectToken({Token::identifier, Token::operator_id}, "identifier");
//   expectToken(Token::l_paren, "left parenthesis");
//   std::vector<std::unique_ptr<ParamDecl>>&& param = acceptToken(Token::r_paren) ? std::vector<std::unique_ptr<ParamDecl>>() : parseParamDeclList();
//   expectToken(Token::r_paren, "right parenthesis");
//   if (!consumeOperator("->")) throw CompilerException(token_.location(),  "error: expected ->");
//   Type* type = parseType();
//   return std::make_unique<FuncDecl>(name, param, type, nullptr);
// }

std::unique_ptr<Decl> Parser::parseDecl() {
  switch(token_.type()) {
  case Token::kw_var: return parseVarDecl();
  case Token::kw_let: return parseLetDecl();
  case Token::kw_func: return parseFuncDecl();
  case Token::kw_extern: return parseExternFuncDecl();
  case Token::kw_struct: return parseStructDecl();
  case Token::kw_typealias: return parseTypeAlias();
  default: throw CompilerException(token_.location(),  "error: unable to parse decl");
  }
}
std::unique_ptr<TypeAlias> Parser::parseTypeAlias() {
  expectToken(Token::kw_typealias, "typealias");
  auto name = expectToken(Token::identifier, "identifier");
  if (!consumeOperator("=")) throw CompilerException(token_.location(),  "expected '='");
  auto type = parseType();
  return std::make_unique<TypeAlias>(name, type);
}

std::unique_ptr<StructDecl> Parser::parseStructDecl() {
  expectToken(Token::kw_struct, "struct");
  auto name = expectToken(Token::identifier, "identifier");
  auto type = parseStructType();
  return std::make_unique<StructDecl>(name, type);
}

std::unique_ptr<Decl> Parser::parseVarDecl() {
  expectToken(Token::kw_var, "var");
  auto name = expectToken(Token::identifier, "identifier");
  Type* type = consumeToken(Token::colon)? parseType(): nullptr;
  if (consumeOperator("=")) {
    auto expr = parseExpr();
    return std::make_unique<VarDecl>(name, type, std::move(expr));
  } else {
    return std::make_unique<UninitializedVarDecl>(name, type);
  }
}

std::unique_ptr<LetDecl> Parser::parseLetDecl() {
  expectToken(Token::kw_let, "let");
  auto name = expectToken(Token::identifier, "identifier");
  Type* type = consumeToken(Token::colon)? parseType(): nullptr;
  if (consumeOperator("=")) {
    auto expr = parseExpr();
    return std::make_unique<LetDecl>(name, type, std::move(expr));
  } else throw CompilerException(token_.location(),  "constants must be initialized at declaration");
}

std::unique_ptr<ParamDecl> Parser::parseParamDecl() {
  auto name = expectToken(Token::identifier, "identifier");
  expectToken(Token::colon, "colon");
  auto type = parseType();
  return std::make_unique<ParamDecl>(name, type);
}

std::vector<std::unique_ptr<ParamDecl>> Parser::parseParamDeclList() {
  std::vector<std::unique_ptr<ParamDecl>> elements;
  elements.push_back(parseParamDecl());
  while (consumeToken(Token::comma)) {
    elements.push_back(parseParamDecl());
  }
  return elements;
}

std::unique_ptr<FuncDecl> Parser::parseFuncDecl() {
  expectToken(Token::kw_func, "func");
  auto name = expectToken({Token::identifier, Token::operator_id}, "identifier");
  expectToken(Token::l_paren, "left parenthesis");
  auto param = acceptToken(Token::r_paren) ? std::vector<std::unique_ptr<ParamDecl>>() : parseParamDeclList();
  expectToken(Token::r_paren, "right parenthesis");
  if (!consumeOperator("->")) throw CompilerException(token_.location(),  "error: expected ->");
  auto type = parseType();
  auto stmt = parseCompoundStmt();
  return std::make_unique<FuncDecl>(name, std::move(param), type, std::move(stmt));
}


std::unique_ptr<ExternFuncDecl> Parser::parseExternFuncDecl() {
  expectToken(Token::kw_extern, "extern");
  expectToken(Token::kw_func, "func");
  auto name = expectToken(Token::identifier, "identifier");
  auto type = parseFunctionType();
  return std::make_unique<ExternFuncDecl>(name, std::move(type));
}
