#include "AST/Type.h"
#include "iostream"

ostream& operator<<(ostream& os, Type* x) {
  if (dynamic_cast<LabeledType*>(x)) {
    auto t = dynamic_cast<LabeledType*>(x);
    os << t->label << ": " << t->type;
  } else if (dynamic_cast<TypeIdentifier*>(x)) {
    auto t = dynamic_cast<TypeIdentifier*>(x);
    os << t->getLexeme() ;
  } else if (dynamic_cast<TupleType*>(x)) {
    auto t = dynamic_cast<TupleType*>(x);
    os << "(" << t->list << ")" ;
  } else if (dynamic_cast<FunctionType*>(x)) {
    auto t = dynamic_cast<FunctionType*>(x);
    os << "(" << t->params << ") -> " << t->returns ;
  } else if (dynamic_cast<ListType*>(x)) {
    auto t = dynamic_cast<ListType*>(x);
    os << "[" << t->type << "]" ;
  } else if (dynamic_cast<MapType*>(x)) {
    auto t = dynamic_cast<MapType*>(x);
    os << "[" << t->keyType << ": " << t->valType << "]" ;
  }
  return os;
}

ostream& operator<<(ostream& os, TypeLabel* x) {
  os << x->getLexeme();
  return os;
}

ostream& operator<<(ostream& os, TypeList* x) {
  if (!x) return os;
  
  if (x->list) {
    os << x->element << ", " << x->list;
  } else {
    os << x->element;
  }
  return os;
}
