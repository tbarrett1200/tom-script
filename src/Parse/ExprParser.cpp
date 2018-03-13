#include "AST/Expr.h"
#include "Parse/Parser.h"

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
  if (token().is(Token::l_paren)) return parseTupleExpr();

  switch(precedence) {
    case 0: return parseValueExpr();
    case 1: return parseUnaryExpr();
    default: return parseBinaryExpr(precedence);
  }
}


unique_ptr<ExprList> Parser::parseExprList() {
  auto expr = parseLabeledExprOrExpr();
  auto comma = token();
  if (consumeToken(Token::comma)) {
    if (acceptToken(Token::r_paren)) throw report(comma, "error: extreneous comma");
    else {
      auto next = parseExprList();
      return make_unique<ExprList>(move(expr), move(next));
    }
  } else return make_unique<ExprList>(move(expr), nullptr);
}

unique_ptr<OperatorExpr> Parser::parseOperatorExpr(int precedence) {
  Token tok = token();
  if (OperatorTable::level(precedence).contains(tok.lexeme)) {
    consume();
    return make_unique<OperatorExpr>(tok);
  } else throw report(token(), "error: expected operator");
}

unique_ptr<Expr> Parser::parseIdentifierOrFunctionCall() {
  auto id = parseIdentifier();
  if (!acceptToken(Token::l_paren)) return id;
  auto tuple = parseFunctionParameters();
  return make_unique<FunctionCall>(move(id), move(tuple));
}

unique_ptr<IdentifierExpr> Parser::parseIdentifier() {
  auto token = expectToken(Token::identifier, "identifier");
  return make_unique<IdentifierExpr>(token);
}

unique_ptr<TupleExpr> Parser::parseFunctionParameters() {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = parseExprList();
  expectToken(Token::r_paren, "right parenthesis");
  return make_unique<TupleExpr>(move(list));
}

unique_ptr<Expr> Parser::parseTupleExpr() {
  expectToken(Token::l_paren, "left parenthesis");
  if (acceptToken(Token::colon)) throw report(token(), "error: expected labeled tuple member");
  auto list = parseExprList();
  expectToken(Token::r_paren, "right parenthesis");
  if (list && list->size() == 1) {
    if (list->has<LabeledExpr>()) {
      auto expr = dynamic_cast<LabeledExpr*>(list->element.get());
      auto label = expr->label->name;
      throw report(label, "error: expressions may not be labeled");
    } else return move(list->element);
  } else {
    return make_unique<TupleExpr>(move(list));
  }
}

unique_ptr<FunctionCall> Parser::parseFunctionCall() {
  auto id = parseIdentifier();
  auto tuple = parseFunctionParameters();
  return make_unique<FunctionCall>(move(id), move(tuple));
}


unique_ptr<Expr> Parser::parseValueExpr() {
  switch (token().getType()) {
  case Token::identifier:
    return parseIdentifierOrFunctionCall();
  case Token::integer_literal:
    return parseIntegerExpr();
  case Token::double_literal:
    return parseDoubleExpr();
  case Token::string_literal:
    return parseStringExpr();
  case Token::l_paren:
    return parseTupleExpr();
  default: throw report(token(), "error: expected value");
  }
}

unique_ptr<Expr> Parser::parseUnaryExpr() {
  if (!OperatorTable::level(1).contains(token().lexeme)) {
    return parseValueExpr();
  } else {
    auto op = parseOperatorExpr(1);
    auto expr = parseValueExpr();
    return make_unique<UnaryExpr>(move(op),move(expr));
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
  if (!OperatorTable::level(p).contains(token().lexeme)) return left;
  auto op = parseOperatorExpr(p);
  auto right = parseExpr(p-1);
  return make_unique<BinaryExpr>(move(left), move(op), move(right));
}

unique_ptr<Expr> Parser::parseInfixRight(int p) {
  auto left = parseExpr(p-1);
  if (!OperatorTable::level(p).contains(token().lexeme)) return left;
  auto op = parseOperatorExpr(p);
  auto right = parseExpr(p);
  return make_unique<BinaryExpr>(move(left), move(op), move(right));
}

unique_ptr<Expr> Parser::parseInfixLeft(int precedence) {
  auto left = parseExpr(precedence-1);
  function<unique_ptr<Expr>(int,unique_ptr<Expr>)> continueParse;
  continueParse = [this, &continueParse](int precedence, unique_ptr<Expr> left) -> unique_ptr<Expr> {
    if (!OperatorTable::level(precedence).contains(token().lexeme)) return left;
    auto op = parseOperatorExpr(precedence);
    auto right = parseExpr(precedence-1);
    return continueParse(precedence, make_unique<BinaryExpr>(move(left),move(op),move(right)));
  };
  return continueParse(precedence, move(left));
}

unique_ptr<ExprLabel> Parser::parseExprLabel() {
  auto token = expectToken(Token::identifier, "label");
  return make_unique<ExprLabel>(token);
}

unique_ptr<LabeledExpr> Parser::parseLabeledExpr() {
  auto label = parseExprLabel();
  expectToken(Token::colon, "colon");
  auto expr = parseExpr();
  return make_unique<LabeledExpr>(move(label), move(expr));
}

unique_ptr<Expr> Parser::parseLabeledExprOrExpr() {
  if (token().is(Token::identifier) && token(1).is(Token::colon)) return parseLabeledExpr();
  else return parseExpr();
}

unique_ptr<IntegerExpr> Parser::parseIntegerExpr() {
  auto token = expectToken(Token::integer_literal, "integer literal");
  return make_unique<IntegerExpr>(token);
}

unique_ptr<DoubleExpr> Parser::parseDoubleExpr() {
  auto token = expectToken(Token::double_literal, "double literal");
  return make_unique<DoubleExpr>(token);
}

unique_ptr<StringExpr> Parser::parseStringExpr() {
  auto token = expectToken(Token::string_literal, "string literal");
  return make_unique<StringExpr>(token);
}
