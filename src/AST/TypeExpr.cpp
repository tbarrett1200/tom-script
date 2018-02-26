#include "AST/TypeExpr.h"
#include <stdexcept>

using namespace std;

TypeIdentifier::TypeIdentifier(Token n) {
  name = n;
}
TypeLabel::TypeLabel(Token n) {
  name = n;
}
LabeledType::LabeledType(unique_ptr<TypeLabel> p, unique_ptr<TypeLabel> s, unique_ptr<Type> t)
: primary{move(p)}, secondary{move(s)}, type{move(t)} {
  if (!type) {
    throw std::domain_error("labeled-type: type is required");
  }
}

LabeledTypeList::LabeledTypeList(unique_ptr<LabeledType> e, unique_ptr<LabeledTypeList> l)
: element{move(e)}, list{move(l)} {
  if (!element) {
    throw std::domain_error("type-list: element is required");
  }
}

TupleType::TupleType(unique_ptr<LabeledTypeList> l)
: list{move(l)} {
}

FunctionType::FunctionType(unique_ptr<LabeledTypeList> p, unique_ptr<Type> r)
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
