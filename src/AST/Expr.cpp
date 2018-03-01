#include "AST/Expr.h"

std::vector<int> Expr::startTokens = {
  Token::identifier,
  Token::integer_literal,
  Token::double_literal,
  Token::string_literal,
  Token::l_paren
};

ExprLabel::ExprLabel(Token n) : name{n} {}

LabeledExpr::LabeledExpr(unique_ptr<ExprLabel> l, unique_ptr<Expr> e)
: label{move(l)}, expr{move(e)} {
  if (!label) {
    throw std::domain_error("labeled expr: label is required");
  }
  if (!expr) {
    throw std::domain_error("labeled expr: expr is required");
  }
}


OperatorExpr::OperatorExpr(Token t) : token{t} {
  if (t.isNot(Token::operator_id)) {
    throw std::domain_error("OperatorExpr requires a token of type operator_id");
  }
}

BinaryExpr::BinaryExpr(unique_ptr<Expr> l, unique_ptr<OperatorExpr> o, unique_ptr<Expr> r)
: left{move(l)}, op{move(o)}, right{move(r)} {
  if (!left) {
    throw std::domain_error("BinaryExpr: left is required");
  }
  if (!op) {
    throw std::domain_error("BinaryExpr: op is required");
  }
  if (!right) {
    throw std::domain_error("BinaryExpr: right is required");
  }
}
UnaryExpr::UnaryExpr(unique_ptr<OperatorExpr> o, unique_ptr<Expr> e) : op{move(o)}, expr{move(e)} {
  if (!op) {
    throw std::domain_error("BinaryExpr: op is required");
  }
  if (!expr) {
    throw std::domain_error("BinaryExpr: expr is required");
  }
}

StringExpr::StringExpr(Token t) : token{t} {
  if (t.isNot(Token::string_literal)) {
    throw std::domain_error("StringExpr requires a token of type string_literal");
  }
}
IntegerExpr::IntegerExpr(Token t) : token{t} {
  if (t.isNot(Token::integer_literal)) {
    throw std::domain_error("IntegerExpr requires a token of type integer_literal");
  }
}
DoubleExpr::DoubleExpr(Token t) : token{t} {
  if (t.isNot(Token::double_literal)) {
    throw std::domain_error("DoubleExpr requires a token of type double_literal");
  }
}
IdentifierExpr::IdentifierExpr(Token t) : token{t} {
  if (t.isNot(Token::identifier)) {
    throw std::domain_error("Identifier requires a token of type identifier");
  }
}
TupleExpr::TupleExpr(unique_ptr<ExprList> l) : list{move(l)} {

}
FunctionCall::FunctionCall(unique_ptr<IdentifierExpr> n, unique_ptr<TupleExpr> a)
: name{move(n)}, arguments{move(a)} {
  if (!name) {
    throw std::domain_error("function call: name is required");
  }
}
