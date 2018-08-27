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

IntegerType IntegerType::singleton;

const IntegerType* IntegerType::getInstance() {
  return &IntegerType::singleton;
}


CharacterType CharacterType::singleton;

const CharacterType* CharacterType::getInstance() {
  return &CharacterType::singleton;
}


//----------------------------------------------------------------------------//
// IntegerType
//----------------------------------------------------------------------------//

DoubleType DoubleType::singleton;


const DoubleType* DoubleType::getInstance() {
  return &DoubleType::singleton;
}

BooleanType BooleanType::singleton;


const BooleanType* BooleanType::getInstance() {
  return &BooleanType::singleton;
}

//----------------------------------------------------------------------------//
// TypeIdentifier
//----------------------------------------------------------------------------//

std::vector<std::unique_ptr<PointerType>> PointerType::instances;
std::vector<std::unique_ptr<SliceType>> SliceType::instances;
std::vector<std::unique_ptr<StructType>> StructType::instances;
std::vector<std::unique_ptr<ReferenceType>> ReferenceType::instances;
std::vector<std::unique_ptr<TypeIdentifier>> TypeIdentifier::instances;
std::vector<std::unique_ptr<TupleType>> TupleType::instances;
std::vector<std::unique_ptr<FunctionType>> FunctionType::instances;
std::vector<std::unique_ptr<ListType>> ListType::instances;
std::vector<std::unique_ptr<MapType>> MapType::instances;

bool equal(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2) {
  return t1->getCanonicalType() == t2->getCanonicalType();
}
