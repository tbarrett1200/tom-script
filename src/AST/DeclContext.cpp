#include "AST/DeclContext.h"
#include "AST/Decl.h"

std::shared_ptr<DeclContext> DeclContext::globalContext = std::make_shared<DeclContext>(nullptr);

void DeclContext::addDecl(class Decl* d) {
  fDecls[d->getName()] = d;
}
