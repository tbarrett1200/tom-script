#ifndef SYMBOL_CHECKER_H
#define SYMBOL_CHECKER_H

#include "Visitor.h"

class SymbolChecker: public Visitor {
private:
  SourceCode *source;
public:
  SymbolChecker(SourceCode*);
  void visit(Identifier*);
  void visit(VarDecl*);
  void visit(FuncDecl*);
};

#endif
