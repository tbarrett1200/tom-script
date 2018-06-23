#ifndef AST_AST_WALKER_H
#define AST_AST_WALKER_H

#include "AST/Decl.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"
#include "AST/Type.h"

#include <memory>

/// A user overridable preorder abstract syntax tree traverser
class ASTWalker {
public:

  void traverse(std::shared_ptr<TreeElement> m) {
    if (std::dynamic_pointer_cast<Decl>(m)) traverseDecl(std::dynamic_pointer_cast<Decl>(m));
    else if (std::dynamic_pointer_cast<Expr>(m)) traverseExpr(std::dynamic_pointer_cast<Expr>(m));
    else if (std::dynamic_pointer_cast<Stmt>(m)) traverseStmt(std::dynamic_pointer_cast<Stmt>(m));
    else if (std::dynamic_pointer_cast<Type>(m)) traverseType(std::dynamic_pointer_cast<Type>(m));
    else {
      for(auto child: m->getChildren()) {
        traverse(child);
      }
    }
  }

  void traverseDecl(std::shared_ptr<Decl> d) {
    switch (d->getKind()) {
    #define DECL(SELF, SUPER) case Decl::Kind::SELF: traverse##SELF(std::dynamic_pointer_cast<SELF>(d)); break;
    #include "AST/Decl.def"
    #undef DECL
    }
  }
  void traverseExpr(std::shared_ptr<Expr> e) {
    switch (e->getKind()) {
    #define EXPR(SELF, SUPER) case Expr::Kind::SELF: traverse##SELF(std::dynamic_pointer_cast<SELF>(e)); break;
    #include "AST/Expr.def"
    #undef EXPR
    }
  }
  void traverseStmt(std::shared_ptr<Stmt> s) {
    switch (s->getKind()) {
    #define STMT(SELF, SUPER) case Stmt::Kind::SELF: traverse##SELF(std::dynamic_pointer_cast<SELF>(s)); break;
    #include "AST/Stmt.def"
    #undef STMT
    }
  }
  void traverseType(std::shared_ptr<Type> t) {
    switch (t->getKind()) {
    #define TYPE(SELF, SUPER) case Type::Kind::SELF: traverse##SELF(std::dynamic_pointer_cast<SELF>(t)); break;
    #include "AST/Type.def"
    #undef TYPE
    }
  }

  #define DECL(SELF, SUPER) \
  void traverse##SELF(std::shared_ptr<SELF> x) { \
    if (walkUpFrom##SELF(x)) \
      for(auto child: x->getChildren()) \
        traverse(child); \
  }
  #include "AST/Decl.def"
  #undef DECL

  #define EXPR(SELF, SUPER)               \
  void traverse##SELF(std::shared_ptr<SELF> x) {          \
    if (walkUpFrom##SELF(x)) \
      for(auto child: x->getChildren()) \
        traverse(child); \
  }
  #include "AST/Expr.def"
  #undef EXPR

  #define STMT(SELF, SUPER)               \
  void traverse##SELF(std::shared_ptr<SELF> x) {          \
    if (walkUpFrom##SELF(x)) \
        for(auto child: x->getChildren()) \
          traverse(child); \
  }
  #include "AST/Stmt.def"
  #undef STMT

  #define TYPE(SELF, SUPER)               \
  void traverse##SELF(std::shared_ptr<SELF> x) {          \
    if (walkUpFrom##SELF(x)) \
      for(auto child: x->getChildren()) \
          traverse(child); \
  }
  #include "AST/Type.def"
  #undef TYPE


  bool walkUpFromDecl(std::shared_ptr<Decl> d) {
    return visitDecl(d);
  }
  bool walkUpFromExpr(std::shared_ptr<Expr> e) {
    return visitExpr(e);
  }
  bool walkUpFromStmt(std::shared_ptr<Stmt> s) {
    return visitStmt(s);
  }
  bool walkUpFromType(std::shared_ptr<Type> t) {
    return visitType(t);
  }

  #define DECL(SELF, SUPER)                         \
  bool walkUpFrom##SELF(std::shared_ptr<SELF> x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Decl.def"
  #undef DECL

  #define EXPR(SELF, SUPER)                         \
  bool walkUpFrom##SELF(std::shared_ptr<SELF> x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Expr.def"
  #undef EXPR

  #define STMT(SELF, SUPER)                         \
  bool walkUpFrom##SELF(std::shared_ptr<SELF> x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Stmt.def"
  #undef STMT

  #define TYPE(SELF, SUPER)                         \
  bool walkUpFrom##SELF(std::shared_ptr<SELF> x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Type.def"
  #undef TYPE


  virtual bool visitDecl(std::shared_ptr<Decl> d) { return true; }
  virtual bool visitExpr(std::shared_ptr<Expr> e) { return true; }
  virtual bool visitStmt(std::shared_ptr<Stmt> s) { return true; }
  virtual bool visitType(std::shared_ptr<Type> t) { return true; }

  #define DECL(SELF, SUPER) virtual bool visit##SELF(std::shared_ptr<SELF> x) { return true; }
  #include "AST/Decl.def"
  #undef DECL

  #define EXPR(SELF, SUPER) virtual bool visit##SELF(std::shared_ptr<SELF> x) { return true; }
  #include "AST/Expr.def"
  #undef EXPR

  #define STMT(SELF, SUPER) virtual bool visit##SELF(std::shared_ptr<SELF> x) { return true; }
  #include "AST/Stmt.def"
  #undef STMT

  #define TYPE(SELF, SUPER) virtual bool visit##SELF(std::shared_ptr<SELF> x) { return true; }
  #include "AST/Type.def"
  #undef TYPE

};

#endif
