#include "AST/DeclContext.h"
#include "AST/Decl.h"
#include "AST/Type.h"

#include <algorithm>

DeclContext DeclContext::globalContext;


Decl* DeclContext::getDecl(const FunctionSignature &signature) {
  auto candidate_iterator = decls_.equal_range(signature.name());
  std::vector<std::pair<const StringRef, Decl*>> candidates;

  std::copy_if(candidate_iterator.first, candidate_iterator.second, std::back_inserter(candidates),
  [&signature](std::pair<const StringRef, Decl*> pair) {
    if (const FunctionType *func_type = dynamic_cast<const FunctionType*>(pair.second->getType())) {
      if (func_type->getParamCount() == signature.params().size()) {
        for (int i = 0; i < func_type->getParamCount(); i++) {
          const Type* t1 = func_type->getParam(i);
          const Type* t2 = signature.params()[i];
          if (t1 != t2) return false;
        }
        return true;
      } else return false;
    } else return false;
  });

  if (candidates.size() > 1) {
    std::stringstream ss;
    ss << "ambigious lookup of '" << signature.name() << "'";
    throw CompilerException(signature.name().start, ss.str());
  } else if (candidates.size() == 1) {
    return candidates.front().second;
  } else if (parent_ == nullptr) {
    return nullptr;
  } else return parent_->getDecl(signature);
}
//
//
// Decl* DeclContext::getDecl(StringRef name, const std::vector<std::unique_ptr<Expr>> &arguments) {
//   auto candidate_iterator = decls_.equal_range(name);
//   std::vector<std::pair<const StringRef, Decl*>> candidates;
//
//   std::copy_if(candidate_iterator.first, candidate_iterator.second, std::back_inserter(candidates),
//   [&arguments](std::pair<const StringRef, Decl*> pair) {
//     if (const FunctionType *func_type = dynamic_cast<const FunctionType*>(pair.second->getType())) {
//       if (func_type->getParamCount() == arguments.size()) {
//         for (int i = 0; i < func_type->getParamCount(); i++) {
//           const Type* t1 = func_type->getParam(i);
//           const Type* t2 = signature.params()[i];
//           if (t1 != t2) return false;
//         }
//         return true;
//       } else return false;
//     } else return false;
//   });
//
//   if (candidates.size() > 1) {
//     std::stringstream ss;
//     ss << "ambigious lookup of '" << signature.name() << "'";
//     throw CompilerException(signature.name().start, ss.str());
//   } else if (candidates.size() == 1) {
//     return candidates.front().second;
//   } else if (parent_ == nullptr) {
//     return nullptr;
//   } else return parent_->getDecl(signature);
// }

void DeclContext::addDecl(Decl* d) {
  decls_.insert(std::pair<StringRef, Decl*>(d->getName(), d));
}
