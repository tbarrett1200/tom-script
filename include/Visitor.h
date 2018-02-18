#ifndef VISITOR_H
#define VISITOR_H

#include "Tree.h"
#include "ErrorReporter.h"

class Visitor {
public:
   virtual void visit(IntLiteral*);
   virtual void visit(DoubleLiteral*);
   virtual void visit(StringLiteral*);
   virtual void visit(Identifier*);
   virtual void visit(Type*);
   virtual void visit(OperatorNode*);
   virtual void visit(BinaryExpr*);
   virtual void visit(UnaryExpr*);
   virtual void visit(StmtList*);
   virtual void visit(ExprList*);
   virtual void visit(BlockStmt*);
   virtual void visit(VarDecl*);
   virtual void visit(FunctionCall*);
   virtual void visit(FuncDecl*);
   virtual void visit(IfStmt*);
   virtual void visit(WhileStmt*);
   virtual void visit(ExprStmt*);
   virtual void visit(ReturnStmt*);
   virtual void visit(Program*);
};

class PrintVisitor: public Visitor {
public:
  static int indent;
  void visit(IntLiteral*);
  void visit(DoubleLiteral*);
  void visit(StringLiteral*);
  void visit(Identifier*);
  void visit(BinaryExpr* t);
  void visit(UnaryExpr* t);
  void visit(Type*);
  void visit(OperatorNode*);
  void visit(BlockStmt*);
  void visit(StmtList*);
  void visit(VarDecl*);
  void visit(FuncDecl*);
  void visit(FunctionCall*);
  void visit(ExprList*);
  void visit(IfStmt*);
  void visit(WhileStmt*);
  void visit(ExprStmt*);
  void visit(ReturnStmt*);
  void visit(Program*);
};

#endif
