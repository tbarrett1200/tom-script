#include "AST/Expr.h"
#include "Parse/Parser.h"
#include "Parse/Scope.h"
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


shared_ptr<Expr> Parser::parseExpr(int precedence) {
  if (token().is(Token::l_paren)) return parseTupleExpr();

  switch(precedence) {
    case 0: return parseValueExpr();
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
std::vector<std::shared_ptr<Expr>> Parser::parseExprList() {
  std::vector<std::shared_ptr<Expr>> elements;
  elements.push_back(parseExpr());
  while (consumeToken(Token::comma)) {
    elements.push_back(parseExpr());
  }
  return elements;
}

Token Parser::parseOperator(int precedence) {
  Token tok = token();
  if (OperatorTable::level(precedence).contains(tok.lexeme)) {
    consume();
    return tok;
  } else throw CompilerException(token().getLocation(),  "error: expected operator");
}

shared_ptr<Expr> Parser::parseIdentifierOrFunctionCallOrAccessor() {
  auto id = parseIdentifier();
  if (acceptToken(Token::l_square)) {
    expectToken(Token::l_square, "[");
    auto index = parseIntegerExpr();
    expectToken(Token::r_square, "]");
    return std::make_shared<AccessorExpr>(id, index);
  } else if (acceptToken(Token::l_paren)) {
    auto tuple = parseFunctionParameters();
    return make_shared<FunctionCall>(move(id), move(tuple));
  } else {
    return id;
  }
}


shared_ptr<IdentifierExpr> Parser::parseIdentifier() {
  auto token = expectToken({Token::identifier, Token::operator_id}, "identifier");
  auto type = scope.getType(token.lexeme);
  if (type == nullptr) {
    throw CompilerException(token.getLocation(), std::string("undeclared identifier ") + token.lexeme );
  } else {
    return std::make_shared<IdentifierExpr>(token, type);
  }
}

std::vector<std::shared_ptr<Expr>> Parser::parseFunctionParameters() {
  expectToken(Token::l_paren, "left parenthesis");
  if (consumeToken(Token::r_paren)) return {};
  auto list = parseExprList();
  expectToken(Token::r_paren, "right parenthesis");
  return list;
}

shared_ptr<Expr> Parser::parseTupleExpr() {
  expectToken(Token::l_paren, "left parenthesis");
  if (acceptToken(Token::colon)) throw CompilerException(token().getLocation(),  "error: expected labeled tuple member");
  std::vector<std::shared_ptr<Expr>> list = parseExprList();
  expectToken(Token::r_paren, "right parenthesis");
  return make_shared<TupleExpr>(move(list));
}

shared_ptr<FunctionCall> Parser::parseFunctionCall() {
  auto id = parseIdentifier();
  auto tuple = parseFunctionParameters();
  return make_shared<FunctionCall>(move(id), move(tuple));
}


shared_ptr<Expr> Parser::parseValueExpr() {
  switch (token().getType()) {
  case Token::identifier:
    return parseIdentifierOrFunctionCallOrAccessor();
  case Token::integer_literal:
    return parseIntegerExpr();
  case Token::l_square:
    return parseListExpr();
  case Token::double_literal:
    return parseDoubleExpr();
  case Token::string_literal:
    return parseStringExpr();
  case Token::l_paren:
    return parseTupleExpr();
  default: throw CompilerException(SourceManager::currentFile(),
                                   token().getLocation(),
                                   CompilerExceptionCategory::Error,
                                   "expected value but got " + token().lexeme);
  }
}


shared_ptr<Expr> Parser::parseUnaryExpr() {
  if (!OperatorTable::level(1).contains(token().lexeme)) {
    return parseValueExpr();
  } else {
    auto op = parseOperator(1);
    auto expr = parseValueExpr();
    return make_shared<UnaryExpr>(move(op),move(expr), expr->getType());
  }
}

shared_ptr<Expr> Parser::parseBinaryExpr(int precedence) {
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

shared_ptr<Expr> Parser::parseInfixNone(int p) {
  auto left = parseExpr(p-1);
  if (!OperatorTable::level(p).contains(token().lexeme)) return left;
  auto op = parseOperator(p);
  auto right = parseExpr(p-1);
  return make_shared<BinaryExpr>(move(left), move(op), move(right), left->getType());
}

shared_ptr<Expr> Parser::parseInfixRight(int p) {
  auto left = parseExpr(p-1);
  if (!OperatorTable::level(p).contains(token().lexeme)) return left;
  auto op = parseOperator(p);
  auto right = parseExpr(p);
  return make_shared<BinaryExpr>(move(left), move(op), move(right), left->getType());
}

shared_ptr<Expr> Parser::parseInfixLeft(int precedence) {
  auto left = parseExpr(precedence-1);
  function<shared_ptr<Expr>(int,shared_ptr<Expr>)> continueParse;
  continueParse = [this, &continueParse](int precedence, shared_ptr<Expr> left) -> shared_ptr<Expr> {
    if (!OperatorTable::level(precedence).contains(token().lexeme)) return left;
    auto op = parseOperator(precedence);
    auto right = parseExpr(precedence-1);
    return continueParse(precedence, make_shared<BinaryExpr>(move(left),move(op),move(right),left->getType()));
  };
  return continueParse(precedence, move(left));
}

shared_ptr<IntegerExpr> Parser::parseIntegerExpr() {
  auto token = expectToken(Token::integer_literal, "integer literal");
  return make_shared<IntegerExpr>(token);
}

shared_ptr<DoubleExpr> Parser::parseDoubleExpr() {
  auto token = expectToken(Token::double_literal, "double literal");
  return make_shared<DoubleExpr>(token);
}

/**
 *
 *
 */
shared_ptr<ListExpr> Parser::parseListExpr() {
  auto l_square = expectToken(Token::l_square, "[");
  auto &&contents = parseExprList();
  auto r_square = expectToken(Token::r_square, "]");
  return std::make_shared<ListExpr>(std::move(contents));
}

shared_ptr<StringExpr> Parser::parseStringExpr() {
  auto token = expectToken(Token::string_literal, "string literal");
  return make_shared<StringExpr>(token);
}
