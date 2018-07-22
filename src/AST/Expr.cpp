#include "AST/Expr.h"
#include "AST/Type.h"
#include <memory>

//=--------------------------------------------------------------------------=//
// Expr
//=--------------------------------------------------------------------------=//
// EXPR(IntegerExpr, Expr)
// EXPR(DoubleExpr, Expr)
// EXPR(BoolExpr, Expr)
// EXPR(StringExpr, Expr)

// EXPR(IdentifierExpr, Expr)

// EXPR(OperatorExpr, Expr)
// EXPR(UnaryExpr, Expr)
// EXPR(BinaryExpr, Expr)
// EXPR(FunctionCall, Expr)

// EXPR(ListExpr, Expr)
// EXPR(TupleExpr, Expr)
// EXPR(AccessorExpr, Expr)

//=--------------------------------------------------------------------------=//
// IntegerExpr
//=--------------------------------------------------------------------------=//

IntegerExpr::IntegerExpr(int i) : token{std::to_string(i), Token::integer_literal, 0, 0, 0} {
  setType(IntegerType::getInstance());
}

IntegerExpr::IntegerExpr(Token t) : token{t} {
  setType(IntegerType::getInstance());
  if (t.isNot(Token::integer_literal)) {
    throw std::domain_error("IntegerExpr requires a token of type integer_literal");
  }
}

/* Returns a vector of children for easy traversal */
std::string IntegerExpr::getLexeme() const {
  return token.lexeme;
}

Expr::Kind IntegerExpr::getKind() const { return Kind::IntegerExpr; }

int IntegerExpr::getInt() const {
  return std::stoi(token.lexeme);
}

bool IntegerExpr::isLeftValue() const {
  return false;
}

//=--------------------------------------------------------------------------=//
// DoubleExpr
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::string DoubleExpr::getLexeme() const {
  return token.lexeme;
}

Expr::Kind DoubleExpr::getKind() const { return Kind::DoubleExpr; }

bool DoubleExpr::isLeftValue() const {
  return false;
}

double DoubleExpr::getDouble() const {
  return std::stod(token.lexeme);
}


DoubleExpr::DoubleExpr(double i) : token{std::to_string(i), Token::double_literal, 0, 0, 0} {
  setType(DoubleType::getInstance());
}

DoubleExpr::DoubleExpr(Token t) : token{t} {
  setType(DoubleType::getInstance());
  if (t.isNot(Token::double_literal)) {
    throw std::domain_error("DoubleExpr requires a token of type double_literal");
  }
}

//=--------------------------------------------------------------------------=//
// BoolExpr
//=--------------------------------------------------------------------------=//
/* Returns a vector of children for easy traversal */
std::string BoolExpr::getLexeme() const {
  return token.lexeme;
}

Expr::Kind  BoolExpr::getKind() const { return Kind::BoolExpr; }

bool  BoolExpr::getBool() {
  return token.lexeme == "true";
}

bool BoolExpr::isLeftValue() const {
  return false;
}

BoolExpr::BoolExpr(bool b) {
  setType(BooleanType::getInstance());
  if (b) {
    token = Token{"true", Token::kw_true, 0, 0, 0};
  } else {
    token = Token{"false", Token::kw_false, 0, 0, 0};
  }
}

BoolExpr::BoolExpr(Token t) : token{t} {
  setType(BooleanType::getInstance());
  if (!(t.isAny({Token::kw_true, Token::kw_false}))) {
    throw std::domain_error("BoolExpr requires a boolean literal");
  }
}

//=--------------------------------------------------------------------------=//
// StringExpr
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::string StringExpr::getLexeme() const {
  return token.lexeme;
}

Expr::Kind StringExpr::getKind() const { return Kind::StringExpr; }

bool StringExpr::isLeftValue() const {
  return false;
}

std::string StringExpr::getString() const { return token.lexeme.substr(1,token.lexeme.size()-2); }

StringExpr::StringExpr(std::string s) : token{"\"" + s + "\"", Token::string_literal, 0, 0, 0} {}


StringExpr::StringExpr(Token t) : token{t} {
  throw std::logic_error("unimplemented: no type");

  if (t.isNot(Token::string_literal)) {
    throw std::domain_error("StringExpr requires a token of type string_literal");
  }
}

//=--------------------------------------------------------------------------=//
// IdentifierExpr
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::string IdentifierExpr::getLexeme() const {
  return token.lexeme;
}

Expr::Kind IdentifierExpr::getKind() const { return Kind::IdentifierExpr; }

bool IdentifierExpr::isLeftValue() const {
  return true;
}

IdentifierExpr::IdentifierExpr(Token tok, std::shared_ptr<Type> type) : token{tok} {
  setType(type);
}

//=--------------------------------------------------------------------------=//
// TupleExpr
//=--------------------------------------------------------------------------=//

Expr::Kind TupleExpr::getKind() const { return Kind::TupleExpr; }

bool TupleExpr::isLeftValue() const {
  return false;
}

int TupleExpr::size() const { return elements.size(); }

std::shared_ptr<Expr> TupleExpr::operator[] (int x) {
  return elements[x];
}

TupleExpr::TupleExpr(std::vector<std::shared_ptr<Expr>>&& list) : elements{std::move(list)} {
  throw std::logic_error("accessor expressions not yet implemented");
}

//=--------------------------------------------------------------------------=//
// ListExpr
//=--------------------------------------------------------------------------=//


Expr::Kind ListExpr::getKind() const {
  return Kind::ListExpr;
}
bool ListExpr::isLeftValue() const {
  return false;
}
ListExpr::ListExpr(std::vector<std::shared_ptr<Expr>>&& d): elements{std::move(d)} {
  throw std::logic_error("accessor expressions not yet implemented");
}

//=--------------------------------------------------------------------------=//
// AccessorExpr
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<TreeElement>> AccessorExpr::getChildren() const {
  return {id, index};
}

Expr::Kind AccessorExpr::getKind() const {
  return Expr::Kind::AccessorExpr;
}

bool AccessorExpr::isLeftValue() const {
  return true;
}

AccessorExpr::AccessorExpr(std::shared_ptr<IdentifierExpr> a, std::shared_ptr<IntegerExpr> b): id{a}, index{b} {
  throw std::logic_error("accessor expressions not yet implemented");
}


//=--------------------------------------------------------------------------=//
// UnaryExpr
//=--------------------------------------------------------------------------=//
/* Returns a vector of children for easy traversal */
std::vector<std::shared_ptr<TreeElement>> UnaryExpr::getChildren() const {
  return {expr};
}

Expr::Kind UnaryExpr::getKind() const { return Kind::UnaryExpr; }

bool UnaryExpr::isLeftValue() const {
  return false;
}

UnaryExpr::UnaryExpr(Token o, std::shared_ptr<Expr> e, std::shared_ptr<Type> t) : op{std::move(o)}, expr{std::move(e)} {
  setType(t);

  if (!expr) {
    throw std::domain_error("BinaryExpr: expr is required");
  }
}

//=--------------------------------------------------------------------------=//
// BinaryExpr
//=--------------------------------------------------------------------------=//
Expr::Kind BinaryExpr::getKind() const { return Kind::BinaryExpr; }

std::vector<std::shared_ptr<TreeElement>> BinaryExpr::getChildren() const {
  return {left, right};
}

bool BinaryExpr::isLeftValue() const {
  return false;
}

BinaryExpr::BinaryExpr(std::shared_ptr<Expr> l, Token o, std::shared_ptr<Expr> r, std::shared_ptr<Type> t)
: left{std::move(l)}, op{o}, right{std::move(r)} {
  setType(t);
  if (!left) {
    throw std::domain_error("BinaryExpr: left is required");
  }

  if (!right) {
    throw std::domain_error("BinaryExpr: right is required");
  }
}

//=--------------------------------------------------------------------------=//
// FunctionCall
//=--------------------------------------------------------------------------=//
FunctionCall::FunctionCall(std::shared_ptr<IdentifierExpr> n, std::vector<std::shared_ptr<Expr>>&& a)
: name{n}, arguments{std::move(a)} {
  setType(n->getType());
}

bool FunctionCall::isLeftValue() const {
  return false;
}

Expr::Kind FunctionCall::getKind() const { return Kind::FunctionCall; }



std::ostream& operator<<(std::ostream& os, Expr* x) {
  if (dynamic_cast<IdentifierExpr*>(x)) {
    auto t = dynamic_cast<IdentifierExpr*>(x);
    os << t->getLexeme();
  } else if (dynamic_cast<FunctionCall*>(x)) {
    auto t = dynamic_cast<FunctionCall*>(x);
    os << t->name << "function call";
  } else if (dynamic_cast<TupleExpr*>(x)) {
    os << "("<< ")";
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
  } else if (dynamic_cast<ListExpr*>(x)) {
    os  << "[" << "]";
  } else if (x == nullptr) {
    os << "nullptr";
  } else {
    os << "unknown expr";
  }

  return os;
}
