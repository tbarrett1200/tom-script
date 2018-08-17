#ifndef AST_DECL_CONTEXT_H
#define AST_DECL_CONTEXT_H

#include <map>
#include <vector>

#include "Basic/SourceCode.h"

struct DeclContextKey {
  StringRef name;
  std::vector<const class Type*> params;
  bool operator<(const DeclContextKey& key) const {
    if (name.str() == key.name.str()) return params < key.params;
    else return name.str() < key.name.str();
  }
};

class DeclContext {
private:
  static DeclContext globalContext;
  DeclContext *fParent = nullptr;
  std::map<DeclContextKey, class Decl*> fDecls;
public:
  DeclContext() = default;

  static DeclContext* getGlobalContext() {
    return &globalContext;
  }

  DeclContext* getParentContext() const {
    return fParent;
  }

  const std::map<DeclContextKey, class Decl*>& getDeclMap() const {
    return fDecls;
  }

  void setParentContext(DeclContext *parent) {
    fParent = parent;
  }

  void addDecl(class Decl* d);

  Decl* getDecl(StringRef name) {
    auto decl_iterator = fDecls.find({name, {}});
    if (decl_iterator != fDecls.end()) {
      return decl_iterator->second;
    } else if (fParent == nullptr) {
      return nullptr;
    } else if (Decl* decl = fParent->getDecl(name)) {
      return decl;
    } else return nullptr;
  }

  Decl* getDecl(StringRef name, std::vector<const class Type*> params) {
    auto decl_iterator = fDecls.find({name, params});
    if (decl_iterator != fDecls.end()) {
      return decl_iterator->second;
    } else if (fParent == nullptr) {
      return nullptr;
    } else if (Decl* decl = fParent->getDecl(name, params)) {
      return decl;
    } else return nullptr;
  }

  template <typename T> T* getDecl(StringRef name) {
    auto decl_iterator = fDecls.find({name, {}});
    if (decl_iterator != fDecls.end()) {
      T* derivedDecl = dynamic_cast<T*>(decl_iterator->second);
      if (derivedDecl) return derivedDecl;
    } else if (fParent) {
      return fParent->getDecl<T>(name);
    } else return nullptr;
  }


};
#endif
