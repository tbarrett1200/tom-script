#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/DeclarationContext.h"
#include "AST/AmbiguousType.h"
#include "AST/AmbiguousDecl.h"
#include "Parse/Parser.h"
#include <memory>

//=--------------------------------------------------------------------------=//
// Expr
//=--------------------------------------------------------------------------=//

AmbiguousType Expr::getType() const {
  return type;
}

//=--------------------------------------------------------------------------=//
// ExprList
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::vector<std::shared_ptr<Matchable>> ExprList::getChildren() const {
  if (!list) return {element};
  else {
    auto children = list->getChildren();
    children.insert(children.begin(), element);
    return children;
  }
}

std::vector<std::shared_ptr<Expr>> ExprList::vector() const {
  if (!list) {
    return {element};
  } else {
    auto l = list->vector();
    l.insert(l.begin(), element);
    return l;
  }
}

std::shared_ptr<ExprList> ExprList::reverse() const {
  auto v = vector();
  std::reverse(v.begin(),v.end());
  return make_shared<ExprList>(v);
}

int ExprList::size() const {
  if (!list) return 1;
  else return list->size()+1;
}

std::shared_ptr<Expr> ExprList::operator[] (int x) {
  if (x == 0) return element;
  else if (!list || x < 0) throw std::logic_error("out of bounds ExprList[]");
  else return (*list)[x-1];
}

template <typename T> bool ExprList::has() {
  if (list == nullptr) return true;
  else if (!dynamic_cast<T*>(element.get())) return false;
  else return list->has<T>();
};

/* Constructor */
ExprList::ExprList(shared_ptr<Expr> e, shared_ptr<ExprList> l)
  : element{move(e)}, list{move(l)} {
}

ExprList::ExprList(std::vector<shared_ptr<Expr>> v) {
  element = v.front();
  v.erase(v.begin());
  list = v.size() > 0 ? std::make_shared<ExprList>(v) : nullptr;
}

//=--------------------------------------------------------------------------=//
// ExprLabel
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::string ExprLabel::getLexeme() const {
  return name.lexeme;
}

ExprLabel::ExprLabel(Token n): name{n} {};

//=--------------------------------------------------------------------------=//
// LabeledExpr
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::vector<std::shared_ptr<Matchable>> LabeledExpr::getChildren() const {
  return {label, expr};
}

Expr::Kind LabeledExpr::getKind() const { return Kind::LabeledExpr; }


LabeledExpr::LabeledExpr(shared_ptr<ExprLabel> l, shared_ptr<Expr> e): label{move(l)}, expr{move(e)} {
  if (!label) {
    throw std::domain_error("labeled expr: label is required");
  }
  if (!expr) {
    throw std::domain_error("labeled expr: expr is required");
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


std::string StringExpr::getString() const { return token.lexeme.substr(1,token.lexeme.size()-2); }

StringExpr::StringExpr(std::string s) : token{"\"" + s + "\"", Token::string_literal, 0, 0, 0} {
  type = {Parser::makeType("String")};
}


StringExpr::StringExpr(Token t) : token{t} {
  if (t.isNot(Token::string_literal)) {
    type = {Parser::makeType("String")};
    throw std::domain_error("StringExpr requires a token of type string_literal");
  }
}

//=--------------------------------------------------------------------------=//
// IntegerExpr
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::string IntegerExpr::getLexeme() const {
  return token.lexeme;
}

Expr::Kind IntegerExpr::getKind() const { return Kind::IntegerExpr; }

int IntegerExpr::getInt() {
  return std::stoi(token.lexeme);
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


//=--------------------------------------------------------------------------=//
// DoubleExpr
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::string DoubleExpr::getLexeme() const {
  return token.lexeme;
}

Expr::Kind DoubleExpr::getKind() const { return Kind::DoubleExpr; }

double DoubleExpr::getDouble() {
  return std::stod(token.lexeme);
}


DoubleExpr::DoubleExpr(double i) : token{to_string(i), Token::double_literal, 0, 0, 0} {
  type = {Parser::makeType("Double")};
}

DoubleExpr::DoubleExpr(Token t) : token{t} {
  type = {Parser::makeType("Double")};
  if (t.isNot(Token::double_literal)) {
    throw std::domain_error("DoubleExpr requires a token of type double_literal");
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


IdentifierExpr::IdentifierExpr(Token t) : token{t} {}

//=--------------------------------------------------------------------------=//
// TupleExpr
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<Matchable>> TupleExpr::getChildren() const {
  return {list};
}

Expr::Kind TupleExpr::getKind() const { return Kind::TupleExpr; }

int TupleExpr::size() const { return list->size(); }
std::shared_ptr<Expr> TupleExpr::operator[] (int x) {
  return (*list)[x];
}

std::shared_ptr<TupleExpr> TupleExpr::make(std::vector<std::shared_ptr<Expr>> l) {
  return std::make_shared<TupleExpr>(std::make_shared<ExprList>(l));
}

std::shared_ptr<TupleExpr> TupleExpr::make(std::shared_ptr<ExprList> l) {
  return std::make_shared<TupleExpr>(l);
}

TupleExpr::TupleExpr(shared_ptr<ExprList> l) : list{move(l)} {}

//=--------------------------------------------------------------------------=//
// OperatorExpr
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::string OperatorExpr::getLexeme() const {
  return token.lexeme;
}

Expr::Kind OperatorExpr::getKind() const { return Kind::OperatorExpr; }


OperatorExpr::OperatorExpr(Token t) : token{t} {
  if (t.isNot(Token::operator_id)) {
    throw std::domain_error("OperatorExpr requires a token of type operator_id");
  }
}

//=--------------------------------------------------------------------------=//
// UnaryExpr
//=--------------------------------------------------------------------------=//
/* Returns a vector of children for easy traversal */
std::vector<std::shared_ptr<Matchable>> UnaryExpr::getChildren() const {
  return {op, expr};
}

Expr::Kind UnaryExpr::getKind() const { return Kind::UnaryExpr; }



UnaryExpr::UnaryExpr(shared_ptr<OperatorExpr> o, shared_ptr<Expr> e) : op{move(o)}, expr{move(e)} {
  if (!op) {
    throw std::domain_error("BinaryExpr: op is required");
  }
  if (!expr) {
    throw std::domain_error("BinaryExpr: expr is required");
  }
}

//=--------------------------------------------------------------------------=//
// BinaryExpr
//=--------------------------------------------------------------------------=//
Expr::Kind BinaryExpr::getKind() const { return Kind::BinaryExpr; }

std::vector<std::shared_ptr<Matchable>> BinaryExpr::getChildren() const {
  return {left, op, right};
}

BinaryExpr::BinaryExpr(shared_ptr<Expr> l, shared_ptr<OperatorExpr> o, shared_ptr<Expr> r)
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

//=--------------------------------------------------------------------------=//
// FunctionCall
//=--------------------------------------------------------------------------=//
FunctionCall::FunctionCall(shared_ptr<IdentifierExpr> n, shared_ptr<ExprList> a) : name{n}, arguments{a} {}

/* Returns a vector of children for easy traversal */
std::vector<std::shared_ptr<Matchable>> FunctionCall::getChildren() const {
  return {name, arguments};
}

Expr::Kind FunctionCall::getKind() const { return Kind::FunctionCall; }

//=--------------------------------------------------------------------------=//
// StackReference
//=--------------------------------------------------------------------------=//

StackReference::StackReference(int l) : location{l} {}
std::string StackReference::getLexeme() const { return "*"; }
Expr::Kind StackReference::getKind() const { return Kind::StackReference; }




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
