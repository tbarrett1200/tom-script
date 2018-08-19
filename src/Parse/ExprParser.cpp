#include "AST/Expr.h"
#include "Parse/Parser.h"
#include "Basic/CompilerException.h"

#include <functional>
#include <stdexcept>

using namespace std;

std::vector<int> exprStartTokens = {
  Token::identifier,
  Token::integer_literal,
  Token::double_literal,
  Token::string_literal,
  Token::l_paren
};


unique_ptr<Expr> Parser::parseExpr(int precedence) {
  if (token_.is(Token::l_paren)) return parseTupleExpr();

  switch(precedence) {
    case 0: return parseAccessorExpr();
    case 1: return parseUnaryExpr();
    default: return parseBinaryExpr(precedence);
  }
}

/**
 * Parses a comma seperated list of expressions. The expressions are allowed
 * to be labeled. If unable to parse, throws a CompilerException.
 *
 * Note that this function parses iteratively rather than recursively. This is
 * done in an attempt to simplify the processing of lists of items. The ultimate
 * goal is to replace the expression-list recursive definition with a vector
 * of expressions.
 */
std::vector<std::unique_ptr<Expr>> Parser::parseExprList() {
  std::vector<std::unique_ptr<Expr>> elements;
  elements.push_back(parseExpr());
  while (consumeToken(Token::comma)) {
    elements.push_back(parseExpr());
  }
  return elements;
}

Token Parser::parseOperator(int precedence) {
  Token tok = token_;
  if (OperatorTable::level(precedence).contains(tok.lexeme())) {
    consume();
    return tok;
  } else throw CompilerException(tok.location(),  "error: expected operator");
}

std::unique_ptr<Expr> Parser::parseIdentifierOrFunctionCallOrAccessor() {
  auto id = parseIdentifier();
  if (acceptToken(Token::l_square)) {
    expectToken(Token::l_square, "[");
    auto index = parseIntegerExpr();
    expectToken(Token::r_square, "]");
    return std::make_unique<AccessorExpr>(std::move(id), std::move(index));
  } else if (acceptToken(Token::l_paren)) {
    auto args = parseFunctionArguments();
    return make_unique<FunctionCall>(std::move(id), std::move(args));
  } else {
    return id;
  }
}

std::unique_ptr<IdentifierExpr> Parser::parseIdentifier() {
  auto token = expectToken({Token::identifier, Token::operator_id}, "identifier");
  return std::make_unique<IdentifierExpr>(token);
}

std::vector<std::unique_ptr<Expr>> Parser::parseFunctionArguments() {
  expectToken(Token::l_paren, "left parenthesis");
  if (consumeToken(Token::r_paren)) return {};
  auto list = parseExprList();
  expectToken(Token::r_paren, "right parenthesis");
  return list;
}

unique_ptr<Expr> Parser::parseTupleExpr() {
  expectToken(Token::l_paren, "left parenthesis");
  if (acceptToken(Token::colon)) throw CompilerException(token_.location(),  "error: expected labeled tuple member");
  std::vector<std::unique_ptr<Expr>> list = parseExprList();
  expectToken(Token::r_paren, "right parenthesis");
  return make_unique<TupleExpr>(move(list));
}

unique_ptr<FunctionCall> Parser::parseFunctionCall() {
  auto id = parseIdentifier();
  auto tuple = parseFunctionArguments();
  return make_unique<FunctionCall>(move(id), move(tuple));
}


unique_ptr<Expr> Parser::parseValueExpr() {
  switch (token_.type()) {
  case Token::identifier:
    return parseIdentifierOrFunctionCallOrAccessor();
  case Token::integer_literal:
    return parseIntegerExpr();
  case Token::l_square:
    return parseListExpr();
  case Token::character_literal:
    return parseCharacterExpr();
  case Token::kw_true:
  case Token::kw_false:
    return parseBoolExpr();
  case Token::double_literal:
    return parseDoubleExpr();
  case Token::string_literal:
    return parseStringExpr();
  case Token::l_paren:
    return parseTupleExpr();
  default:
    std::stringstream ss;
    ss << "expected value but got '" << token_.lexeme() << "'";
    throw CompilerException(token_.location(), ss.str());
  }
}

std::unique_ptr<Expr> Parser::parseAccessorExpr() {
  std::unique_ptr<Expr> expr = parseValueExpr();
  if (acceptToken(Token::dot)) {
    consume();
    std::unique_ptr<IntegerExpr> index = parseIntegerExpr();
    return std::make_unique<AccessorExpr>(std::move(expr), std::move(index));
  } else return expr;
}

unique_ptr<Expr> Parser::parseUnaryExpr() {
  if (!OperatorTable::level(1).contains(token_.lexeme())) {
    return parseAccessorExpr();
  } else {
    auto op = parseOperator(1);
    auto expr = parseAccessorExpr();
    return std::make_unique<UnaryExpr>(op, std::move(expr));
  }
}

unique_ptr<Expr> Parser::parseBinaryExpr(int precedence) {
  switch (OperatorTable::associativity(precedence)) {
    case Associativity::left:
      return parseInfixLeft(precedence);
    case Associativity::right:
      return parseInfixRight(precedence);
    case Associativity::none:
      return parseInfixNone(precedence);
    default:
      throw domain_error("unable to parse unknown associativity");
  }
}

unique_ptr<Expr> Parser::parseInfixNone(int p) {
  auto left = parseExpr(p-1);
  if (!OperatorTable::level(p).contains(token_.lexeme())) return left;
  auto op = parseOperator(p);
  auto right = parseExpr(p-1);
  return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

unique_ptr<Expr> Parser::parseInfixRight(int p) {
  auto left = parseExpr(p-1);
  if (!OperatorTable::level(p).contains(token_.lexeme())) return left;
  auto op = parseOperator(p);
  auto right = parseExpr(p);
  return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

unique_ptr<Expr> Parser::parseInfixLeft(int precedence) {
  auto left = parseExpr(precedence-1);
  function<unique_ptr<Expr>(int,unique_ptr<Expr>)> continueParse;
  continueParse = [this, &continueParse](int precedence, unique_ptr<Expr> left) -> unique_ptr<Expr> {
    if (!OperatorTable::level(precedence).contains(token_.lexeme())) return left;
    auto op = parseOperator(precedence);
    auto right = parseExpr(precedence-1);
    return continueParse(precedence, std::make_unique<BinaryExpr>(std::move(left), op, std::move(right)));
  };
  return continueParse(precedence, move(left));
}

unique_ptr<IntegerExpr> Parser::parseIntegerExpr() {
  auto token = expectToken(Token::integer_literal, "integer literal");
  return make_unique<IntegerExpr>(token);
}

unique_ptr<DoubleExpr> Parser::parseDoubleExpr() {
  auto token = expectToken(Token::double_literal, "double literal");
  return make_unique<DoubleExpr>(token);
}


unique_ptr<BoolExpr> Parser::parseBoolExpr() {
  if (token_.is(Token::kw_true)) {
    return make_unique<BoolExpr>(expectToken(Token::kw_true, "true"));
  } else if (token_.is(Token::kw_false)) {
    return make_unique<BoolExpr>(expectToken(Token::kw_false, "false"));
  } else {
    throw CompilerException(token_.location(), "expected 'true' or 'false'");
  }
}

unique_ptr<ListExpr> Parser::parseListExpr() {
  expectToken(Token::l_square, "[");
  std::vector<std::unique_ptr<Expr>> contents = parseExprList();
   expectToken(Token::r_square, "]");
  return std::make_unique<ListExpr>(std::move(contents));
}

unique_ptr<StringExpr> Parser::parseStringExpr() {
  auto token = expectToken(Token::string_literal, "string literal");
  return make_unique<StringExpr>(token);
}


std::unique_ptr<CharacterExpr> Parser::parseCharacterExpr() {
  auto token = expectToken(Token::character_literal, "character literal");
  return make_unique<CharacterExpr>(token);
}
