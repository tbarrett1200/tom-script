#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/DeclarationContext.h"
#include "Parse/Parser.h"
#include <memory>

std::shared_ptr<TupleExpr> make(std::vector<std::shared_ptr<Expr>> l) {
  return std::make_shared<TupleExpr>(std::make_shared<ExprList>(l));
}

std::shared_ptr<TupleExpr> make(std::shared_ptr<ExprList> l) {
  return std::make_shared<TupleExpr>(l);
}

AmbiguousType StringExpr::getType(DeclarationContext *c) const {
  return {Parser::makeType("String")};
}

AmbiguousType IntegerExpr::getType(DeclarationContext *c) const {
  return {Parser::makeType("Int")};
}

AmbiguousType DoubleExpr::getType(DeclarationContext *c) const {
  return {Parser::makeType("Double")};
}

AmbiguousType BoolExpr::getType(DeclarationContext *c) const {
  return {Parser::makeType("Bool")};
}

AmbiguousType IdentifierExpr::getType(DeclarationContext *c) const {
  return c->filter([this](std::shared_ptr<Decl> d) {
    return d->getName() == this->getLexeme();
  }).getTypes();
}

AmbiguousType TupleExpr::getType(DeclarationContext *c) const {
  return {TupleType::make(list->getTypeList(c))};
}

std::shared_ptr<TypeList> ExprList::getTypeList(DeclarationContext *c) const {
  return make_shared<TypeList>(element->getType(c).get(), list ? list->getTypeList(c) : nullptr);
}

AmbiguousType OperatorExpr::getType(DeclarationContext *c) const {
  return c->filter([this](std::shared_ptr<Decl> d) {
    return d->getName() == this->getLexeme()
        && d->as<FuncDecl>();
  }).getTypes();
}

AmbiguousType LabeledExpr::getType(DeclarationContext* c) const {
  auto types = expr->getType(c).map<std::shared_ptr<Type>>([this](std::shared_ptr<Type> t){
    return std::make_shared<LabeledType>(std::make_shared<TypeLabel>(this->label->name), t);
  });
  return {types};
}

AmbiguousType UnaryExpr::getType(DeclarationContext *c) const {
  auto function_type = op->getType(c).filter([this, c](std::shared_ptr<Type> t) {
    return t->as<FunctionType>()->params->size() == 1
        && this->expr->getType(c).contains((*t->as<FunctionType>()->params)[0]);
  });

  auto return_type = function_type.map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
    return t->as<FunctionType>()->returns;
  });

  return return_type;
}

AmbiguousType BinaryExpr::getType(DeclarationContext *c) const {
  auto function_type = op->getType(c).filter([this, c](std::shared_ptr<Type> t) {
    return t->as<FunctionType>()->params->size() == 2
        && this->left->getType(c).contains((*t->as<FunctionType>()->params)[0])
        && this->right->getType(c).contains((*t->as<FunctionType>()->params)[1]);
  });

  auto return_type = function_type.map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
    return t->as<FunctionType>()->returns;
  });

  return return_type;
}

AmbiguousType FunctionCall::getType(DeclarationContext *c) const {
  auto function_decl = c->filter([this](std::shared_ptr<Decl> d){
    return this->name->getLexeme() == d->getName() && d->getType()->as<FunctionType>();
  }).getTypes();

  auto function_type = function_decl.filter([this, c](std::shared_ptr<Type> t) {
    if (!this->arguments || !t->as<FunctionType>()->params)
      return !this->arguments && !t->as<FunctionType>()->params;
    else if (this->arguments->size() == t->as<FunctionType>()->params->size())
      return t->as<FunctionType>()->params->matches(*this->arguments->getTypeList(c));
    else
      return false;
  });

  auto return_type = function_type.map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
    return t->as<FunctionType>()->returns;
  });

  return return_type;
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
