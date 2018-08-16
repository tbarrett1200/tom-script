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

  void traverse(TreeElement* m) {
    if (m != nullptr) {
      willTraverseNode(*m);
      if (Decl *d = dynamic_cast<Decl*>(m)) {
        traverseDecl(*d);
      } else if (Expr *e = dynamic_cast<Expr*>(m)) {
        traverseExpr(*e);
      } else if (Stmt *s = dynamic_cast<Stmt*>(m)) {
        traverseStmt(*s);
      }
      didTraverseNode(*m);
    }
  }

  virtual void didTraverseNode(TreeElement& m) {};
  virtual void willTraverseNode(TreeElement& m) {};

  void traverseDecl(Decl& d) {
    switch (d.getKind()) {
    #define DECL(SELF, SUPER) case Decl::Kind::SELF: traverse##SELF(dynamic_cast<SELF&>(d)); break;
    #include "AST/Decl.def"
    #undef DECL
    }
  }
  void traverseExpr(Expr& e) {
    switch (e.getKind()) {
    #define EXPR(SELF, SUPER) case Expr::Kind::SELF: traverse##SELF(dynamic_cast<SELF&>(e)); break;
    #include "AST/Expr.def"
    #undef EXPR
    }
  }
  void traverseStmt(Stmt& s) {
    switch (s.getKind()) {
    #define STMT(SELF, SUPER) case Stmt::Kind::SELF: traverse##SELF(dynamic_cast<SELF&>(s)); break;
    #include "AST/Stmt.def"
    #undef STMT
    }
  }

  #define DECL(SELF, SUPER) \
  void traverse##SELF(SELF& x) { \
    if (walkUpFrom##SELF(x)) \
      for (TreeElement *child: x.getChildren()) \
        traverse(child); \
  }
  #include "AST/Decl.def"
  #undef DECL

  #define EXPR(SELF, SUPER) \
  void traverse##SELF(SELF& x) { \
    if (walkUpFrom##SELF(x)) \
      for(TreeElement *child: x.getChildren()) \
        traverse(child); \
  }
  #include "AST/Expr.def"
  #undef EXPR

  #define STMT(SELF, SUPER) \
  void traverse##SELF(SELF& x) { \
    if (walkUpFrom##SELF(x)) \
        for(TreeElement *child: x.getChildren()) \
          traverse(child); \
  }
  #include "AST/Stmt.def"
  #undef STMT

  bool walkUpFromDecl(Decl& d) {
    return visitDecl(d);
  }
  bool walkUpFromExpr(Expr& e) {
    return visitExpr(e);
  }
  bool walkUpFromStmt(Stmt& s) {
    return visitStmt(s);
  }

  #define DECL(SELF, SUPER)                         \
  bool walkUpFrom##SELF(SELF& x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Decl.def"
  #undef DECL

  #define EXPR(SELF, SUPER)                         \
  bool walkUpFrom##SELF(SELF& x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Expr.def"
  #undef EXPR

  #define STMT(SELF, SUPER)                         \
  bool walkUpFrom##SELF(SELF& x) {                  \
    return walkUpFrom##SUPER(x) && visit##SELF(x);  \
  }
  #include "AST/Stmt.def"
  #undef STMT


  virtual bool visitDecl(Decl& d) { return true; }
  virtual bool visitExpr(Expr& e) { return true; }
  virtual bool visitStmt(Stmt& s) { return true; }

  #define DECL(SELF, SUPER) virtual bool visit##SELF(SELF& x) { return true; }
  #include "AST/Decl.def"
  #undef DECL

  #define EXPR(SELF, SUPER) virtual bool visit##SELF(SELF& x) { return true; }
  #include "AST/Expr.def"
  #undef EXPR

  #define STMT(SELF, SUPER) virtual bool visit##SELF(SELF& x) { return true; }
  #include "AST/Stmt.def"
  #undef STMT

};

#endif
