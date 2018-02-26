#ifndef AST_TYPE_DECL
#define AST_TYPE_DECL

#include "AST/ASTNode.h"
#include "AST/Expr.h"
#include "Parse/Token.h"

using namespace std;

class Type : public Expr {
public:
};

class TypeLabel {
public:
  Token name;
  TypeLabel(Token);
};

class LabeledType : public Type {
public:
  unique_ptr<TypeLabel> primary;
  unique_ptr<TypeLabel> secondary;
  unique_ptr<Type> type;
  bool hasLabel();
  LabeledType(unique_ptr<TypeLabel>, unique_ptr<TypeLabel>, unique_ptr<Type>);
  ~LabeledType() {};
};

class LabeledTypeList {
public:
  unique_ptr<LabeledType> element;
  unique_ptr<LabeledTypeList> list;
  LabeledTypeList(unique_ptr<LabeledType>, unique_ptr<LabeledTypeList>);
  ~LabeledTypeList() = default;
};

class TypeIdentifier : public Type {
public:
  Token name;
  TypeIdentifier(Token);
  ~TypeIdentifier() = default;
};

class TupleType : public Type {
  unique_ptr<LabeledTypeList> list;
public:
  TupleType(unique_ptr<LabeledTypeList>);
  ~TupleType() = default;
};

class FunctionType : public Type {
public:
  unique_ptr<LabeledTypeList> params;
  unique_ptr<Type> returns;
  FunctionType(unique_ptr<LabeledTypeList>, unique_ptr<Type>);
  ~FunctionType() = default;
};

class ListType : public Type {
public:
  unique_ptr<Type> type;
  ListType(unique_ptr<Type>);
  ~ListType() = default;
};

class MapType : public Type {
public:
  unique_ptr<Type> keyType;
  unique_ptr<Type> valType;
  MapType(unique_ptr<Type>, unique_ptr<Type>);
  ~MapType() = default;
};

#endif
