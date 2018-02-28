#include "AST/Expr.h"
#include "Parse/Parser.h"

#include <functional>

unique_ptr<Expr> Parser::parseExpr(int precedence) {
  if (token().is(Token::l_paren)) return parseTupleExpr();

  switch(precedence) {
    case 0: return parseValueExpr();
    case 1: return parseUnaryExpr();
    default: return parseBinaryExpr(precedence);
  }
}

unique_ptr<ExprList> Parser::parseExprList() {
  if (!token().isAny(Expr::startTokens)) {
    return nullptr;
  };

  auto expr = parseLabeledExpr();

  if (!expr) {
    report(token(), "Error: expr-list: expected expression");
    return nullptr;
  }

  if (parseTerminal(Token::comma, ",", false)) {
    auto next = parseExprList();
    if (!next) {
      report(token(), "Error: expr_list: extreneous comma");
      return make_unique<ExprList>(move(expr), nullptr);;
    } else return make_unique<ExprList>(move(expr), move(next));
  } else return make_unique<ExprList>(move(expr), nullptr);
}


unique_ptr<OperatorExpr> Parser::parseOperatorExpr(int precedence) {
  Token tok = token();
  if (OperatorTable::level(precedence).contains({tok.lexeme})) {
    consume();
    return make_unique<OperatorExpr>(tok);
  } else return nullptr;
}

unique_ptr<Expr> Parser::parseIdentifierOrFunctionCall() {
  auto id = parseIdentifier();
  if (!id) {
    report(token(), "error: expected identifier");
    return nullptr;
  }
  if (!token().is(Token::l_paren)) return id;
  auto tuple = parseTupleExpr();
  if (!tuple) {
    report(token(), "error: expected params");
    return nullptr;
  }
  return make_unique<FunctionCall>(move(id), move(tuple));
}

unique_ptr<Identifier> Parser::parseIdentifier() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return make_unique<Identifier>(tok);
  } else {
    report(token(), "error: expected expr");
    return nullptr;
  }
}

unique_ptr<TupleExpr> Parser::parseTupleExpr() {
  if (!parseTerminal(Token::l_paren, "(", false)) return nullptr;
  auto list = parseExprList();
  if (!parseTerminal(Token::r_paren, ")", false)) return nullptr;
  return make_unique<TupleExpr>(move(list));
}

unique_ptr<FunctionCall> Parser::parseFunctionCall() {
  auto id = parseIdentifier();
  if (!id) {
    report(token(), "error: expected identifier");
    return nullptr;
  }
  auto tuple = parseTupleExpr();
  if (!tuple) {
    report(token(), "error: expected params");
    return nullptr;
  }
  return make_unique<FunctionCall>(move(id), move(tuple));
}


unique_ptr<Expr> Parser::parseValueExpr() {
  if (token().is(Token::identifier)) {
    return parseIdentifierOrFunctionCall();
  } else if (token().isIntLiteral()) {
    return parseIntegerExpr();
  } else if (token().isDoubleLiteral()) {
    return parseDoubleExpr();
  } else if (token().is(Token::string_literal)) {
    return parseStringExpr();
  } else if (token().is(Token::l_paren)) {
    if (!parseTerminal(Token::l_paren, "(", false)) return nullptr;
    auto expr = parseExpr();
    if (!expr) {
      report(token(), "Error: expected expression");
      return nullptr;
    }
    if (!parseTerminal(Token::r_paren, ")", false)) return nullptr;
    return expr;
  } else return nullptr;
}

unique_ptr<Expr> Parser::parseUnaryExpr() {
  auto op = parseOperatorExpr(1);
  auto expr = parseValueExpr();
  if (!expr) {
    report(token(), "Error: unary-expr: expected expression");
    return nullptr;
  }
  if (!op) return expr;
  return make_unique<UnaryExpr>(move(op),move(expr));
}

unique_ptr<Expr> Parser::parseBinaryExpr(int precedence) {
  switch (OperatorTable::associativity(precedence)) {
    case Associativity::left:
      return parseInfixLeft(precedence);
    case Associativity::right:
      return parseInfixRight(precedence);
    case Associativity::none:
      return parseInfixNone(precedence);
  }
}

unique_ptr<Expr> Parser::parseInfixNone(int p) {
  auto left = parseExpr(p-1);
  if (!left) return nullptr;
  auto op = parseOperatorExpr(p);
  if (!op) return left;
  auto right = parseExpr(p-1);
  if (!right) {
    report(token(), "error: infix-none: expected expression");
    return nullptr;
  }
  return make_unique<BinaryExpr>(move(left), move(op), move(right));
}

unique_ptr<Expr> Parser::parseInfixRight(int p) {
  auto left = parseExpr(p-1);
  if (!left) return nullptr;
  auto op = parseOperatorExpr(p);
  if (!op) return left;
  auto right = parseExpr(p);
  if (!right) {
    report(token(), "error: expected expression");
    return nullptr;
  }
  return make_unique<BinaryExpr>(move(left), move(op), move(right));
}

unique_ptr<Expr> Parser::parseInfixLeft(int precedence) {
  auto left = parseExpr(precedence-1);
  if (!left) return nullptr;
  function<unique_ptr<Expr>(int,unique_ptr<Expr>)> continueParse;
  continueParse = [this, &continueParse](int precedence, unique_ptr<Expr> left) -> unique_ptr<Expr> {
    auto op = parseOperatorExpr(precedence);
    if (!op) return left;
    auto right = parseExpr(precedence-1);
    if (!right) {
      report(token(), "error: infix-expr: expected expression");
      return nullptr;
    }
    return continueParse(precedence, make_unique<BinaryExpr>(move(left),move(op),move(right)));
  };
  return continueParse(precedence, move(left));
}

unique_ptr<ExprLabel> Parser::parseExprLabel() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return make_unique<ExprLabel>(tok);
  } else {
    report(token(), "error: expected expr");
    return nullptr;
  }
}

unique_ptr<Expr> Parser::parseLabeledExpr() {
  if (!token(1).is(Token::colon)) return parseExpr();
  auto label = parseExprLabel();
  if (!label) {
    report(token(), "error: expected label");
    return nullptr;
  }
  consume();
  auto expr = parseExpr();
  if (!expr) {
    report(token(), "error: expected type");
    return nullptr;
  }
  return make_unique<LabeledExpr>(move(label), move(expr));
}

unique_ptr<IntegerExpr> Parser::parseIntegerExpr() {
  Token tok = token();
  if (tok.is(Token::number) && tok.isIntLiteral()) {
    consume();
    return make_unique<IntegerExpr>(tok);
  } else {
    return nullptr;
  }
}

unique_ptr<DoubleExpr> Parser::parseDoubleExpr() {
  Token tok = token();
  if (tok.is(Token::number) && tok.isDoubleLiteral()) {
    consume();
    return make_unique<DoubleExpr>(tok);
  } else {
    return nullptr;
  }
}

unique_ptr<StringExpr> Parser::parseStringExpr() {
  Token tok = token();
  if (tok.is(Token::string_literal)) {
    consume();
    return make_unique<StringExpr>(tok);
  } else {
    return nullptr;
  }
}
