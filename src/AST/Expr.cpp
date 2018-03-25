#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/DeclarationContext.h"
#include "AST/AmbiguousType.h"
#include "AST/AmbiguousDecl.h"
#include "Parse/Parser.h"
#include <memory>


std::shared_ptr<TupleExpr> TupleExpr::make(std::vector<std::shared_ptr<Expr>> l) {
  return std::make_shared<TupleExpr>(std::make_shared<ExprList>(l));
}

std::shared_ptr<TupleExpr> TupleExpr::make(std::shared_ptr<ExprList> l) {
  return std::make_shared<TupleExpr>(l);
}



ostream& operator<<(ostream& os, Expr* x) {
  if (dynamic_cast<IdentifierExpr*>(x)) {
    auto t = dynamic_cast<IdentifierExpr*>(x);
    os << t->getLexeme();
  } else if (dynamic_cast<FunctionCall*>(x)) {
    auto t = dynamic_cast<FunctionCall*>(x);
    os << t->name << t->arguments;
  } else if (dynamic_cast<TupleExpr*>(x)) {
    auto t = dynamic_cast<TupleExpr*>(x);
    os << "(" << t->list << ")";
  } else if (dynamic_cast<IntegerExpr*>(x)) {
    auto t = dynamic_cast<IntegerExpr*>(x);
    os  << t->token.lexeme ;
  } else if (dynamic_cast<DoubleExpr*>(x)) {
    auto t = dynamic_cast<DoubleExpr*>(x);
    os  << t->token.lexeme ;
  } else if (dynamic_cast<BoolExpr*>(x)) {
    auto t = dynamic_cast<BoolExpr*>(x);
    os  << t->token.lexeme ;
  } else if (dynamic_cast<StringExpr*>(x)) {
    auto t = dynamic_cast<StringExpr*>(x);
    os  << t->token.lexeme ;
  } else if (dynamic_cast<FunctionCall*>(x)) {
    auto t = dynamic_cast<FunctionCall*>(x);
    os  << t->name << "(args)";
  } else if (dynamic_cast<StackReference*>(x)) {
    auto t = dynamic_cast<StackReference*>(x);
    os  << "*" << t->location ;
  } else {
    os << "expr";
  }

  return os;
}

StringExpr::StringExpr(std::string s) : token{"\"" + s + "\"", Token::string_literal, 0, 0, 0} {
  type = {Parser::makeType("String")};
}


StringExpr::StringExpr(Token t) : token{t} {
  if (t.isNot(Token::string_literal)) {
    type = {Parser::makeType("String")};
    throw std::domain_error("StringExpr requires a token of type string_literal");
  }
}

IntegerExpr::IntegerExpr(int i) : token{to_string(i), Token::integer_literal, 0, 0, 0} {
  type = {Parser::makeType("Int")};
}

IntegerExpr::IntegerExpr(Token t) : token{t} {
  type = {Parser::makeType("Int")};
  if (t.isNot(Token::integer_literal)) {
    throw std::domain_error("IntegerExpr requires a token of type integer_literal");
  }
}

BoolExpr::BoolExpr(bool b) {
  type = {Parser::makeType("Bool")};

  if (b) {
    token = Token{"true", Token::kw_true, 0, 0, 0};
  } else {
    token = Token{"false", Token::kw_false, 0, 0, 0};
  }
}

BoolExpr::BoolExpr(Token t) : token{t} {
  type = {Parser::makeType("Bool")};
  if (!(t.isAny({Token::kw_true, Token::kw_false}))) {
    throw std::domain_error("BoolExpr requires a boolean literal");
  }
}

DoubleExpr::DoubleExpr(int i) : token{to_string(i), Token::double_literal, 0, 0, 0} {
  type = {Parser::makeType("Double")};
}

DoubleExpr::DoubleExpr(Token t) : token{t} {
  type = {Parser::makeType("Double")};
  if (t.isNot(Token::double_literal)) {
    throw std::domain_error("DoubleExpr requires a token of type double_literal");
  }
}

ostream& operator<<(ostream& os, ExprList* x) {
  if (!x) return os;

  if (x->list) {
    os << x->element << ", " << x->list;
  } else {
    os << x->element;
  }
  return os;
}

ostream& operator<<(ostream& os, ExprLabel* x) {
  os << x->getLexeme();
  return os;
}
