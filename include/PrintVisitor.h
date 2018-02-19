#ifndef PRINT_VISITOR_H
#define PRINT_VISITOR_H

#include "Visitor.h"

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
