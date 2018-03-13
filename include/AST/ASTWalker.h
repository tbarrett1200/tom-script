#ifndef AST_AST_WALKER_H
#define AST_AST_WALKER_H

#include "AST/Decl.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"
#include "AST/Type.h"

class ASTWalker {
public:

  void traverse(Matchable* m) {
    if (dynamic_cast<Decl*>(m)) traverseDecl(dynamic_cast<Decl*>(m));
    else if (dynamic_cast<Expr*>(m)) traverseExpr(dynamic_cast<Expr*>(m));
    else if (dynamic_cast<Stmt*>(m)) traverseStmt(dynamic_cast<Stmt*>(m));
    else if (dynamic_cast<Type*>(m)) traverseType(dynamic_cast<Type*>(m));
    else if (dynamic_cast<NonTerminal*>(m)){
      for(auto child: dynamic_cast<NonTerminal*>(m)->getChildren()) {
        traverse(child);
      }
    }
  }

  void traverseDecl(Decl* d) {
    switch (d->getKind()) {
    #define DECL(SELF, SUPER) case Decl::Kind::SELF: traverse##SELF(dynamic_cast<SELF*>(d)); break;
    #include "AST/Decl.def"
    #undef DECL
    }
  }
  void traverseExpr(Expr* e) {
    switch (e->getKind()) {
    #define EXPR(SELF, SUPER) case Expr::Kind::SELF: traverse##SELF(dynamic_cast<SELF*>(e)); break;
    #include "AST/Expr.def"
    #undef EXPR
    }
  }
  void traverseStmt(Stmt* s) {
    switch (s->getKind()) {
    #define STMT(SELF, SUPER) case Decl::Kind::SELF: traverse##SELF(dynamic_cast<SELF*>(d)); break;
    #include "AST/Stmt.def"
    #undef STMT
    }
  }
  void traverseType(Type* t) {
    switch (t->getKind()) {
    #define TYPE(SELF, SUPER) case Type::Kind::SELF: traverse##SELF(dynamic_cast<SELF*>(t)); break;
    #include "AST/Type.def"
    #undef TYPE
    }
  }

  #define DECL(SELF, SUPER) \
  void traverse##SELF(SELF* x) { \
    if (walkUpFrom##SELF(x)) \
      if (dynamic_cast<NonTerminal*>(x)) \
        for(auto child: dynamic_cast<NonTerminal*>(x)->getChildren()) \
          traverse(child); \
  }
  #include "AST/Decl.def"
  #undef DECL

  #define EXPR(SELF, SUPER)               \
  void traverse##SELF(SELF* x) {          \
    if (walkUpFrom##SELF(x)) \
      if (dynamic_cast<NonTerminal*>(x)) \
        for(auto child: dynamic_cast<NonTerminal*>(x)->getChildren()) \
          traverse(child); \
  }
  #include "AST/Expr.def"
  #undef EXPR

  #define STMT(SELF, SUPER)               \
  void traverse##SELF(SELF* x) {          \
    if (walkUpFrom##SELF(x)) \
      if (dynamic_cast<NonTerminal*>(x)) \
        for(auto child: dynamic_cast<NonTerminal*>(x)->getChildren()) \
          traverse(child); \
  }
  #include "AST/Stmt.def"
  #undef STMT

  #define TYPE(SELF, SUPER)               \
  void traverse##SELF(SELF* x) {          \
    if (walkUpFrom##SELF(x)) \
      if (dynamic_cast<NonTerminal*>(x)) \
        for(auto child: dynamic_cast<NonTerminal*>(x)->getChildren()) \
          traverse(child); \
  }
  #include "AST/Type.def"
  #undef TYPE


  bool walkUpFromDecl(Decl* d) {
    return visitDecl(d);
  }
  bool walkUpFromExpr(Expr* e) {
    return visitExpr(e);
  }
  bool walkUpFromStmt(Stmt* s) {
    return visitStmt(s);
  }
  bool walkUpFromType(Type* t) {
    return visitType(t);
  }

  #define DECL(SELF, SUPER)                         \
  bool walkUpFrom##SELF(SELF* x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Decl.def"
  #undef DECL

  #define EXPR(SELF, SUPER)                         \
  bool walkUpFrom##SELF(SELF* x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Expr.def"
  #undef EXPR

  #define STMT(SELF, SUPER)                         \
  bool walkUpFrom##SELF(SELF* x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Stmt.def"
  #undef STMT

  #define TYPE(SELF, SUPER)                         \
  bool walkUpFrom##SELF(SELF* x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Type.def"
  #undef TYPE


  virtual bool visitDecl(Decl* d) { return true; }
  virtual bool visitExpr(Expr* e) { return true; }
  virtual bool visitStmt(Stmt* s) { return true; }
  virtual bool visitType(Type* t) { return true; }

  #define DECL(SELF, SUPER) virtual bool visit##SELF(SELF* x) { return true; }
  #include "AST/Decl.def"
  #undef DECL

  #define EXPR(SELF, SUPER) virtual bool visit##SELF(SELF* x) { return true; }
  #include "AST/Expr.def"
  #undef EXPR

  #define STMT(SELF, SUPER) virtual bool visit##SELF(SELF* x) { return true; }
  #include "AST/Stmt.def"
  #undef STMT

  #define TYPE(SELF, SUPER) virtual bool visit##SELF(SELF* x) { return true; }
  #include "AST/Type.def"
  #undef TYPE

};

#endif
