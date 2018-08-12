#ifndef AST_DECL_CONTEXT_H
#define AST_DECL_CONTEXT_H

#include <string>
#include <map>

class DeclContext {
private:
  static DeclContext globalContext;
  DeclContext *fParent;
  std::map<std::string, class Decl*> fDecls;
public:
  DeclContext() = default;

  static DeclContext* getGlobalContext() {
    return &globalContext;
  }

  DeclContext* getParentContext() const {
    return fParent;
  }

  const std::map<std::string, class Decl*>& getDeclMap() const {
    return fDecls;
  }

  void setParentContext(DeclContext *parent) {
    fParent = parent;
  }

  void addDecl(class Decl* d);

  Decl* getDecl(std::string name) {
    auto decl_iterator = fDecls.find(name);
    if (decl_iterator != fDecls.end()) {
      return decl_iterator->second;
    } else if (fParent == nullptr) {
      return nullptr;
    } else if (Decl* decl = fParent->getDecl(name)) {
      return decl;
    } else return nullptr;
  }

  template <typename T> T* getDecl(std::string name) {
    auto decl_iterator = fDecls.find(name);
    if (decl_iterator != fDecls.end()) {
      T* derivedDecl = dynamic_cast<T*>(decl_iterator->second);
      if (derivedDecl) return derivedDecl;
    } else if (fParent) {
      return fParent->getDecl<T>(name);
    } else return nullptr;
  }

};
#endif
