#ifndef AST_DECL_CONTEXT_H
#define AST_DECL_CONTEXT_H

#include <string>
#include <map>

class DeclContext {
private:
  static std::shared_ptr<DeclContext> globalContext;
  DeclContext *fParent;
  std::map<std::string, class Decl*> fDecls;
public:

  DeclContext(DeclContext *aParent) : fParent{ aParent ? aParent: globalContext.get() } {}

  static DeclContext *getGlobalContext() {
    return globalContext.get();
  }

  DeclContext *getParentContext() const {
    return fParent;
  }

  void setParentContext(DeclContext *parent) {
    fParent = parent;
  }

  void addDecl(class Decl* d);

  template <typename T> T* getDecl(std::string name) {
    Decl* decl = fDecls[name];
    if (decl != nullptr) {
      T* derivedDecl = dynamic_cast<T*>(decl);
      if (derivedDecl) return derivedDecl;
    } else if (fParent) {
      return fParent->getDecl<T>(name);
    } else return nullptr;
  }

};
#endif
