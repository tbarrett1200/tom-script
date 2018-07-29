#include "AST/Type.h"
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

bool Type::isBooleanType() const {
  return getCanonicalType()->getKind() == Kind::BooleanType;
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

std::shared_ptr<BooleanType> BooleanType::singleton = std::make_shared<BooleanType>();


std::shared_ptr<BooleanType> BooleanType::getInstance() {
  return BooleanType::singleton;
}

std::shared_ptr<PointerType> PointerType::singleton = std::make_shared<PointerType>();


std::shared_ptr<PointerType> PointerType::getInstance() {
  return PointerType::singleton;
}
//----------------------------------------------------------------------------//
// TypeIdentifier
//----------------------------------------------------------------------------//

std::string TypeIdentifier::getName() const {
  return token.lexeme;
}

std::ostream& operator<<(std::ostream& os, Type* x) {
  os << "Type";
  return os;
}

bool equal(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2) {
  return t1->getCanonicalType() == t2->getCanonicalType();
}
