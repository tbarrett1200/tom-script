#ifndef VISITOR_H
#define VISITOR_H

#include "Tree.h"
#include "ErrorReporter.h"

class Visitor {
public:
   virtual void visit(IntLiteral*) = 0;
   virtual void visit(DoubleLiteral*) = 0;
   virtual void visit(StringLiteral*) = 0;
   virtual void visit(Identifier*) = 0;
   virtual void visit(Type*) = 0;
   virtual void visit(Operator*) = 0;
   virtual void visit(BinaryExpr*) = 0;
   virtual void visit(StmtList*) = 0;
   virtual void visit(BlockStmt*) = 0;
   virtual void visit(VarDecl*) = 0;
   virtual void visit(ExprList*) = 0;
   virtual void visit(FunctionCall*) = 0;
   virtual void visit(FuncDecl*) = 0;
   virtual void visit(IfStmt*) = 0;
   virtual void visit(WhileStmt*) = 0;
   virtual void visit(ExprStmt*) = 0;
   virtual void visit(ReturnStmt*) = 0;
   virtual void visit(Program*) = 0;
};

class PrintVisitor: public Visitor {
public:
  void visit(IntLiteral*);
  void visit(DoubleLiteral*);
  void visit(StringLiteral*);
  void visit(Identifier*);
  void visit(Type*);
  void visit(Operator*);
  void visit(BinaryExpr*);
  void visit(StmtList*);
  void visit(BlockStmt*);
  void visit(VarDecl*);
  void visit(ExprList*);
  void visit(FunctionCall*);
  void visit(FuncDecl*);
  void visit(IfStmt*);
  void visit(WhileStmt*);
  void visit(ExprStmt*);
  void visit(ReturnStmt*);
  void visit(Program*);
};

#endif
