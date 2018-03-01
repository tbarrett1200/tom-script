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

using TypeList = List<Type>;

class LabeledType : public Type {
public:
  unique_ptr<TypeLabel> label;
  unique_ptr<Type> type;
  bool hasLabel();
  LabeledType(unique_ptr<TypeLabel>, unique_ptr<Type>);
};

class TypeIdentifier : public Type {
public:
  Token name;
  TypeIdentifier(Token);
};

class TupleType : public Type {
  unique_ptr<TypeList> list;
public:
  TupleType(unique_ptr<TypeList>);
};

class FunctionType : public Type {
public:
  unique_ptr<TypeList> params;
  unique_ptr<Type> returns;
  FunctionType(unique_ptr<TypeList>, unique_ptr<Type>);
};

class ListType : public Type {
public:
  unique_ptr<Type> type;
  ListType(unique_ptr<Type>);
};

class MapType : public Type {
public:
  unique_ptr<Type> keyType;
  unique_ptr<Type> valType;
  MapType(unique_ptr<Type>, unique_ptr<Type>);
};

#endif
