#include "Parse/Parser.h"
#include "AST/Decl.h"
#include "AST/Type.h"
#include "Basic/CompilerException.h"

#include <memory>


std::shared_ptr<Decl> Parser::makeDecl(std::string text) {
  const std::stringstream sstream{text};
  auto source = SourceCode{sstream, "factory"};
  auto parser = Parser{&source};
  auto type = parser.parseDecl();
  if (!type) throw CompilerException(type->getLocation(), "parse error");
  return type;
}

std::shared_ptr<Decl> Parser::makeTypeDecl(std::string text) {
  const std::stringstream sstream{text};
  auto source = SourceCode{sstream, "factory"};
  auto parser = Parser{&source};
  auto type = parser.parseTypeDecl();
  if (!type) throw CompilerException(type->getLocation(), "parse error");
  return type;
}

std::shared_ptr<Decl> Parser::makeFuncDecl(std::string text) {
  const std::stringstream sstream{text};
  auto source = SourceCode{sstream, "factory"};
  auto parser = Parser{&source};
  auto type = parser.parseUndefFuncDecl();
  if (!type) throw CompilerException(type->getLocation(), "parse error");
  return type;
}


std::shared_ptr<TypeDecl> Parser::parseTypeDecl() {
  expectToken(Token::kw_typedef, "typedef");
  auto name = expectToken(Token::identifier, "identifier");
  return std::make_shared<TypeDecl>(name);
}

std::shared_ptr<FuncDecl> Parser::parseUndefFuncDecl() {
  expectToken(Token::kw_func, "func");
  auto name = expectToken({Token::identifier, Token::operator_id}, "identifier");
  expectToken(Token::l_paren, "left parenthesis");
  auto param = acceptToken(Token::r_paren) ? nullptr : parseParamDeclList();
  expectToken(Token::r_paren, "right parenthesis");  if (!consumeOperator("->")) throw CompilerException(token().getLocation(),  "error: expected ->");
  auto type = parseType();
  return std::make_shared<FuncDecl>(name, param, type, nullptr);
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
  std::shared_ptr<Expr> expr = consumeOperator("=")? parseExpr(): nullptr;
  if (type || expr) return std::make_shared<VarDecl>(name, move(type), move(expr));
  else throw CompilerException(token().getLocation(),  "expected type or expression");
}

std::shared_ptr<LetDecl> Parser::parseLetDecl() {
  expectToken(Token::kw_let, "let");
  auto name = expectToken(Token::identifier, "identifier");
  std::shared_ptr<Type> type = consumeToken(Token::colon)? parseType(): nullptr;
  if (consumeOperator("=")) {
    auto expr = parseExpr();
    return std::make_shared<LetDecl>(name, move(type), move(expr));
  } else throw CompilerException(token().getLocation(),  "constants must be initialized at declaration");
}

std::shared_ptr<ParamDecl> Parser::parseParamDecl() {
  auto primary = expectToken(Token::identifier, "identifier");
  auto secondary = acceptToken(Token::identifier) ? expectToken(Token::identifier, "identifier") : primary;
  expectToken(Token::colon, "colon");
  auto type = parseType();
  return std::make_shared<ParamDecl>(primary, secondary, type);
}

std::shared_ptr<ParamDeclList> Parser::parseParamDeclList() {
  auto element = parseParamDecl();
  auto list = consumeToken(Token::comma) ? parseParamDeclList() : nullptr;
  return std::make_shared<ParamDeclList>(element, list);
}

std::shared_ptr<FuncDecl> Parser::parseFuncDecl() {
  expectToken(Token::kw_func, "func");
  auto name = expectToken({Token::identifier, Token::operator_id}, "identifier");
  expectToken(Token::l_paren, "left parenthesis");
  auto param = acceptToken(Token::r_paren) ? nullptr : parseParamDeclList();
  expectToken(Token::r_paren, "right parenthesis");
  if (!consumeOperator("->")) throw CompilerException(token().getLocation(),  "error: expected ->");
  auto type = parseType();
  auto stmt = parseCompoundStmt();
  return std::make_shared<FuncDecl>(name, param, type, stmt);
}
