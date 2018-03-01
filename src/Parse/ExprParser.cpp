#include "AST/Expr.h"
#include "Parse/Parser.h"

#include <functional>
#include <stdexcept>

using namespace std;

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
  if (!expr) return nullptr;

  auto comma = token();
  if (consumeToken(Token::comma)) {
    if (acceptToken(Token::r_paren)) {
      report(comma, "error: extreneous comma");
      return nullptr;
    } else {
      auto next = parseExprList();
      if (!next) {
        return nullptr;
      } else return make_unique<ExprList>(move(expr), move(next));
    }
  } else return make_unique<ExprList>(move(expr), nullptr);
}

unique_ptr<OperatorExpr> Parser::parseOperatorExpr(int precedence) {
  Token tok = token();
  if (OperatorTable::level(precedence).contains(tok.lexeme)) {
    consume();
    return make_unique<OperatorExpr>(tok);
  } else {
    report(token(), "error: expected operator");
    return nullptr;
  }
}

unique_ptr<Expr> Parser::parseIdentifierOrFunctionCall() {
  auto id = parseIdentifier();
  if (!id) return nullptr;
  if (!acceptToken(Token::l_paren)) return id;
  auto tuple = parseFunctionParameters();
  if (!tuple) {
    report(token(), "error: expected parameters");
    return nullptr;
  }
  return make_unique<FunctionCall>(move(id), move(tuple));
}

unique_ptr<IdentifierExpr> Parser::parseIdentifier() {
  auto tok = token();
  if (consumeToken(Token::identifier)) {
    return make_unique<IdentifierExpr>(tok);
  } else {
    report(token(), "error: expected identifier");
    return nullptr;
  }
}

unique_ptr<TupleExpr> Parser::parseFunctionParameters() {
  if (!consumeToken(Token::l_paren)) {
    report(token(), "error: expected opening parenthesis");
    return nullptr;
  }
  auto list = parseExprList();
  if (!consumeToken(Token::r_paren)) {
    report(token(), "error: expected closing parenthesis");
    return nullptr;
  }
  return make_unique<TupleExpr>(move(list));
}

unique_ptr<Expr> Parser::parseTupleExpr() {
  if (!consumeToken(Token::l_paren)) {
    report(token(), "error: expected opening parenthesis");
    return nullptr;
  }
  if (acceptToken(Token::colon)) {
    report(token(), "error: expected labeled tuple member");
    return nullptr;
  }
  auto list = parseExprList();
  if (!consumeToken(Token::r_paren)) {
    report(token(), "error: expected closing parenthesis");
    return nullptr;
  }
  if (list && list->size() == 1) {
    if (list->has<LabeledExpr>()) {
      auto expr = dynamic_cast<LabeledExpr*>(list->element.get());
      auto label = expr->label->name;
      report(label, "error: expressions may not be labeled");
      return nullptr;
    } else return move(list->element);
  } else {
    return make_unique<TupleExpr>(move(list));
  }
}

unique_ptr<FunctionCall> Parser::parseFunctionCall() {
  auto id = parseIdentifier();
  if (!id) return nullptr;
  auto tuple = parseFunctionParameters();
  if (!tuple) return nullptr;
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
  default:
    report(token(), "error: expected value");
    return nullptr;
  }
}

unique_ptr<Expr> Parser::parseUnaryExpr() {
  if (!OperatorTable::level(1).contains(token().lexeme)) {
    return parseValueExpr();
  } else {
    auto op = parseOperatorExpr(1);
    if (!op) return nullptr;
    auto expr = parseValueExpr();
    if (!expr) return nullptr;
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
  if (!left) return nullptr;
  if (!OperatorTable::level(p).contains(token().lexeme)) return left;
  auto op = parseOperatorExpr(p);
  if (!op) return nullptr;
  auto right = parseExpr(p-1);
  if (!right) return nullptr;
  return make_unique<BinaryExpr>(move(left), move(op), move(right));
}

unique_ptr<Expr> Parser::parseInfixRight(int p) {
  auto left = parseExpr(p-1);
  if (!left) return nullptr;
  if (!OperatorTable::level(p).contains(token().lexeme)) return left;
  auto op = parseOperatorExpr(p);
  if (!op) return nullptr;
  auto right = parseExpr(p);
  if (!right) return nullptr;
  return make_unique<BinaryExpr>(move(left), move(op), move(right));
}

unique_ptr<Expr> Parser::parseInfixLeft(int precedence) {
  auto left = parseExpr(precedence-1);
  if (!left) return nullptr;
  function<unique_ptr<Expr>(int,unique_ptr<Expr>)> continueParse;
  continueParse = [this, &continueParse](int precedence, unique_ptr<Expr> left) -> unique_ptr<Expr> {
    if (!OperatorTable::level(precedence).contains(token().lexeme)) return left;
    auto op = parseOperatorExpr(precedence);
    if (!op) return nullptr;
    auto right = parseExpr(precedence-1);
    if (!right) return nullptr;
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
    report(token(), "error: expected label");
    return nullptr;
  }
}

unique_ptr<LabeledExpr> Parser::parseLabeledExpr() {
  auto label = parseExprLabel();
  if (!label) return nullptr;
  if (!consumeToken(Token::colon)) {
    report(token(), "error: expected colon");
    return nullptr;
  }
  auto expr = parseExpr();
  if (!expr) return nullptr;
  return make_unique<LabeledExpr>(move(label), move(expr));
}

unique_ptr<Expr> Parser::parseLabeledExprOrExpr() {
  if (token().is(Token::identifier) && token(1).is(Token::colon)) {
    return parseLabeledExpr();
  } else {
    return parseExpr();
  }
}

unique_ptr<IntegerExpr> Parser::parseIntegerExpr() {
  auto tok = token();
  if (consumeToken(Token::integer_literal)) {
    return make_unique<IntegerExpr>(tok);
  } else {
    report(token(), "error: expected integer");
    return nullptr;
  }
}

unique_ptr<DoubleExpr> Parser::parseDoubleExpr() {
  auto tok = token();
  if (consumeToken(Token::double_literal)) {
    return make_unique<DoubleExpr>(tok);
  } else {
    report(token(), "error: expected double");
    return nullptr;
  }
}

unique_ptr<StringExpr> Parser::parseStringExpr() {
  auto tok = token();
  if (consumeToken(Token::string_literal)) {
    return make_unique<StringExpr>(tok);
  } else {
    report(token(), "error: expected string");
    return nullptr;
  }
}
