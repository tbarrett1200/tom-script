#ifndef AST_CONTEXT
#define AST_CONTEXT

#include "AST/Matchable.h"
#include "AST/ContextSearchResult.h"
#include <string>
#include <list>

using namespace std;

class DeclContext {
private:
  DeclContext *parent = nullptr;
  std::vector<shared_ptr<class Decl>> decls = {};
public:
  DeclContext(std::initializer_list<shared_ptr<class Decl>> d): decls{d} {}

  DeclContext* getParent() {
    return parent;
  }

  void addDecl(shared_ptr<Decl> d) {
    if (d) {
      decls.push_back(d);
    }
    else throw std::runtime_error("error: can't add null to decl contet");
  }

  bool hasDecl(const Matchable* pattern, bool recursive);
  bool hasDecl(std::string n, bool recursive);
  ContextSearchResult getDecl(std::string n, bool recursive);
  ContextSearchResult getDecl(Matchable* pattern, bool recursive);
};

#endif
