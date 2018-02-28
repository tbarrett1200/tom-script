#include "AST/Expr.h"

std::vector<int> Expr::startTokens = {
  Token::identifier,
  Token::number,
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
ExprList::ExprList(unique_ptr<Expr> e, unique_ptr<ExprList> l)
: element{move(e)}, list{move(l)} {
  if (!element) {
    throw std::domain_error("expr list: element is required");
  }
}
OperatorExpr::OperatorExpr(Token t) : token{t}, op{t.lexeme} {}

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

StringExpr::StringExpr(Token t) : token{t} {}
IntegerExpr::IntegerExpr(Token t) : token{t} {}
DoubleExpr::DoubleExpr(Token t) : token{t} {}
Identifier::Identifier(Token t) : token{t} {}
TupleExpr::TupleExpr(unique_ptr<ExprList> l) : list{move(l)} {}
FunctionCall::FunctionCall(unique_ptr<Identifier> n, unique_ptr<TupleExpr> a)
: name{move(n)}, arguments{move(a)} {
  if (!name) {
    throw std::domain_error("function call: name is required");
  }
}
