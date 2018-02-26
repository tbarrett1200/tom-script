#ifndef AST_DECL_H
#define AST_DECL_H

#include "Parse/Token.h"
#include "AST/TypeExpr.h"

class Decl {
public:
  Token name;
  Decl(Token);
};

class TypeAlias : public Decl {
public:
  unique_ptr<Type> type;
  TypeAlias(Token, unique_ptr<Type>);
};

class VarDecl : public Decl {
public:
  unique_ptr<Type> type;
  VarDecl(Token, unique_ptr<Type>);
};

class LetDecl : public Decl {
public:
  unique_ptr<Type> type;
  LetDecl(Token, unique_ptr<Type>);
};

class FuncDecl : public Decl {
public:
  unique_ptr<FunctionType> type;
  FuncDecl(Token, unique_ptr<FunctionType>);
};
#endif
