#ifndef AST_DECL_H
#define AST_DECL_H

#include "Parse/Token.h"
#include "AST/TypeExpr.h"

class Decl {
public:
  enum class Kind {
    TypeAlias, VarDecl, LetDecl, FuncDecl
  };
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
  unique_ptr<Expr> expr;
  VarDecl(Token, unique_ptr<Type>);
  VarDecl(Token, unique_ptr<Expr>);

};

class LetDecl : public Decl {
public:
  unique_ptr<Type> type;
  unique_ptr<Expr> expr;
  LetDecl(Token, unique_ptr<Expr>);
  LetDecl(Token, unique_ptr<Type>, unique_ptr<Expr>);

};

class FuncDecl : public Decl {
public:
  unique_ptr<FunctionType> type;
  FuncDecl(Token, unique_ptr<FunctionType>);
};
#endif
