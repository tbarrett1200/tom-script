#ifndef AST_DECL_CONTEXT_H
#define AST_DECL_CONTEXT_H

#include <map>
#include <vector>

#include "Basic/SourceCode.h"
#include "Basic/CompilerException.h"

class Decl;

class FunctionSignature {
private:
  StringRef name_;
  std::vector<class Type*> params_;
public:
  FunctionSignature(StringRef name, std::vector<class Type*> params)
  : name_{name}, params_{std::move(params)} {}

  StringRef name() const {
    return name_;
  }

  const std::vector<class Type*>& params() const {
    return params_;
  }

  bool operator==(const FunctionSignature& sig2) {
    return name_ == sig2.name_
        && params_ == sig2.params_;
  }
};

class DeclContext {
private:
  static DeclContext globalContext;
  DeclContext *parent_ = nullptr;
  std::multimap<StringRef, class Decl*> decls_;
public:
  DeclContext() = default;

  static DeclContext* getGlobalContext() {
    return &globalContext;
  }

  DeclContext* getParentContext() const {
    return parent_;
  }

  const std::multimap<StringRef, class Decl*>& getDeclMap() const {
    return decls_;
  }

  void setParentContext(DeclContext *parent) {
    parent_ = parent;
  }

  void addDecl(class Decl* d);


  Decl* getDecl(StringRef name) {
    auto candidate_iterator = decls_.equal_range(name);
    auto candidate_count = std::distance(candidate_iterator.first, candidate_iterator.second);
    if (candidate_count > 1) {
      std::stringstream ss;
      ss << "ambigious lookup of '" << name << "'";
      throw CompilerException(name.start, ss.str());
    } else if (candidate_count == 1) {
      return candidate_iterator.first->second;
    } else if (parent_ == nullptr) {
      return nullptr;
    } else return parent_->getDecl(name);
  }

  Decl* getDecl(const FunctionSignature &signature);

};
#endif
