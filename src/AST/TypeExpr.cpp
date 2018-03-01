#include "AST/TypeExpr.h"
#include <stdexcept>

using namespace std;

TypeIdentifier::TypeIdentifier(Token n) {
  name = n;
}
TypeLabel::TypeLabel(Token n) {
  name = n;
}
LabeledType::LabeledType(unique_ptr<TypeLabel> p, unique_ptr<Type> t)
: label{move(p)}, type{move(t)} {
  if (!type) {
    throw std::domain_error("labeled-type: type is required");
  }
}


TupleType::TupleType(unique_ptr<TypeList> l)
: list{move(l)} {
}

FunctionType::FunctionType(unique_ptr<TypeList> p, unique_ptr<Type> r)
: params{move(p)}, returns{move(r)} {}

ListType::ListType(unique_ptr<Type> t)
: type{move(t)} {
  if (!type) {
    throw std::domain_error("list-type: type is required");
  }
}

MapType::MapType(unique_ptr<Type> k,unique_ptr<Type> v)
: keyType{move(k)}, valType{move(v)} {
  if (!keyType) {
    throw std::domain_error("map-type: key-type is required");
  }
  if (!valType) {
    throw std::domain_error("map-type: value-type is required");
  }
}
