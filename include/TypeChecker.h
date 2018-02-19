#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "Visitor.h"

class TypeChecker: public Visitor {
private:
  SourceCode *source;
public:
  TypeChecker(SourceCode*);
  void visit(IntLiteral*);
  void visit(DoubleLiteral*);
  void visit(StringLiteral*);
  void visit(Identifier*);
  void visit(UnaryExpr*);
  void visit(BinaryExpr*);
  void visit(VarDecl* t);
};

#endif
