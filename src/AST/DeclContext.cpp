#include "AST/DeclContext.h"
#include "AST/Decl.h"

DeclContext DeclContext::globalContext;
void DeclContext::addDecl(class Decl* d) {
  if (const FunctionType* func_type = dynamic_cast<const FunctionType*>(d->getType()->getCanonicalType())) {
    fDecls[{d->getName(),func_type->getParamTypes()}] = d;
  } else {
    fDecls[{d->getName(),{}}] = d;
  }
}
