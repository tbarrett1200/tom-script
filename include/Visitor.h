#ifndef VISITOR_H
#define VISITOR_H

#include "Tree.h"

class Visitor {
public:
   virtual void visit(IntegerLiteral*) = 0;
   virtual void visit(Identifier*) = 0;
   virtual void visit(Operator*) = 0;
   virtual void visit(BinaryExpr*) = 0;
   virtual void visit(StmtList*) = 0;
   virtual void visit(BlockStmt*) = 0;
   virtual void visit(VarDecl*) = 0;
   virtual void visit(FuncDecl*) = 0;
   virtual void visit(IfStmt*) = 0;
   virtual void visit(WhileStmt*) = 0;
   virtual void visit(ExprStmt*) = 0;
   virtual void visit(ReturnStmt*) = 0;
};

class PrintVisitor: public Visitor {
public:
  void visit(IntegerLiteral*);
  void visit(Identifier*);
  void visit(Operator*);
  void visit(BinaryExpr*);
  void visit(StmtList*);
  void visit(BlockStmt*);
  void visit(VarDecl*);
  void visit(FuncDecl*);
  void visit(IfStmt*);
  void visit(WhileStmt*);
  void visit(ExprStmt*);
  void visit(ReturnStmt*);
};

#endif
