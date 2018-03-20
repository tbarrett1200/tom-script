#ifndef AST_CONTEXT
#define AST_CONTEXT

#include "AST/AmbiguousDecl.h"

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <vector>
#include <string>
#include <memory>

class Decl;
class Expr;
class Type;

class DeclarationContext {
private:
  DeclarationContext *parent = nullptr;
  std::vector<std::shared_ptr<Decl>> elements = {};
public:
  DeclarationContext(std::initializer_list<std::shared_ptr<Decl>> e): elements{e} {}

  void add(std::shared_ptr<Decl> d) {
    elements.push_back(d);
  }

  AmbiguousDecl filter(std::function<bool(std::shared_ptr<Decl>)> func) {
    AmbiguousDecl self = AmbiguousDecl{elements}.filter(func);
    if (self.isEmpty()) {
      return parent ? parent->filter(func) : self;
    } else return self;
  }
};

#endif
