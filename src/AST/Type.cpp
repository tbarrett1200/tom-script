#include "AST/Type.h"
#include "AST/DeclarationContext.h"
#include <iostream>

//----------------------------------------------------------------------------//
// Type
//----------------------------------------------------------------------------//


const Type* Type::getCanonicalType() const {
  return this;
}

bool Type::isIntegerType() const {
  return getCanonicalType()->getKind() == Kind::IntegerType;
}

bool Type::isDoubleType() const {
  return getCanonicalType()->getKind() == Kind::DoubleType;
}


//----------------------------------------------------------------------------//
// IntegerType
//----------------------------------------------------------------------------//

std::shared_ptr<IntegerType> IntegerType::singleton = std::make_shared<IntegerType>();

std::shared_ptr<IntegerType> IntegerType::getInstance() {
  return IntegerType::singleton;
}

//----------------------------------------------------------------------------//
// IntegerType
//----------------------------------------------------------------------------//

std::shared_ptr<DoubleType> DoubleType::singleton = std::make_shared<DoubleType>();


std::shared_ptr<DoubleType> DoubleType::getInstance() {
  return DoubleType::singleton;
}


//----------------------------------------------------------------------------//
// TypeList
//----------------------------------------------------------------------------//

TypeList::TypeList(std::vector<std::shared_ptr<Type>> l) {
  if (l.size() == 0) {
    throw std::runtime_error("type list must have at least one type");
  } else if (l.size() == 1) {
    element = l[0];
    list = nullptr;
  } else {
    element = l[0];
    l.erase(l.begin());
    list = std::make_shared<TypeList>(l);
  }
}

TypeList::TypeList(std::shared_ptr<Type> e, std::shared_ptr<TypeList> l)
: element{move(e)}, list{move(l)} {}

int TypeList::size() const {
  if (!list) return 1;
  else return list->size()+1;
}

std::shared_ptr<Type> TypeList::operator[] (const int index){
  if (index==0) return element;
  else return (*list)[index-1];
}


//----------------------------------------------------------------------------//
// Labeled Type
//----------------------------------------------------------------------------//

LabeledType::LabeledType(std::shared_ptr<TypeLabel> p, std::shared_ptr<Type> t)
  : label{move(p)}, type{move(t)} {}

Type::Kind LabeledType::getKind() const { return Kind::LabeledType; }

//----------------------------------------------------------------------------//
// TypeIdentifier
//----------------------------------------------------------------------------//

std::string TypeIdentifier::getName() const {
  return token.lexeme;
}


std::shared_ptr<TupleType> TupleType::make(std::shared_ptr<TypeList> l) {
  return std::make_shared<TupleType>(l);
}

std::ostream& operator<<(std::ostream& os, Type* x) {
  os << "Type";
  return os;
}

std::ostream& operator<<(std::ostream& os, TypeLabel* x) {
  os << "Type Label";
  return os;
}

std::ostream& operator<<(std::ostream& os, TypeList* x) {
  os << "TypeList";
  return os;
}

bool equal(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2, DeclarationContext *c) {
  if (t1->getKind() != t2->getKind()) return false;
  auto f1 = c->getFundamentalType(t1);
  auto f2 = c->getFundamentalType(t2);
  return *f1 == *f2;
}

bool equal(std::shared_ptr<TypeList> t1, std::shared_ptr<TypeList> t2, DeclarationContext *c) {
  return t1->size() == t2->size()
      && equal(t1->element, t2->element, c)
      && t1->list ? equal(t1->list, t2->list, c) : true;
}

bool operator != (const Type& l, const Type& r) {
  return !(l==r);
}

bool operator == (const Type& l, const Type& r) {
  return l.getCanonicalType() == r.getCanonicalType();
}

bool operator == (const TypeList& l, const TypeList& r) {
  return false;
}
