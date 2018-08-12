#include "AST/DeclContext.h"
#include "AST/Decl.h"

DeclContext DeclContext::globalContext;
void DeclContext::addDecl(class Decl* d) {
  fDecls[d->getName()] = d;
}
