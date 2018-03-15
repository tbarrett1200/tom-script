#include "AST/ContextSearchResult.h"
#include "AST/AmbiguousType.h"
#include "AST/Decl.h"

AmbiguousType ContextSearchResult::getType() const {
  std::vector<std::shared_ptr<Type>> types;
  for (auto decl: matches) {
    types.push_back(decl->getType());
  }
  return {types};
}
