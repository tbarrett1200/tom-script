#ifndef SYMBOL_ANNOTATOR_H
#define SYMBOL_ANNOTATOR_H

#include "Visitor.h"

class SymbolAnnotator: public Visitor {
public:
  void visit(IntLiteral*);
  void visit(DoubleLiteral*);
  void visit(StringLiteral*);
  void visit(Identifier*);
  void visit(Type*);
  void visit(OperatorNode*);
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
