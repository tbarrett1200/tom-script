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


#endif
