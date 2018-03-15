#ifndef AST_CONTEXT_SEARCH_RESULT_H
#define AST_CONTEXT_SEARCH_RESULT_H

#include <vector>
#include <memory>

class AmbiguousType;

class ContextSearchResult {
public:
  std::vector<std::shared_ptr<class Decl>> localMatches;
  std::vector<std::shared_ptr<class Decl>> matches;
  bool isAmbiguous() const {
    if (matches.size() > 1) return true;
    return false;
  }

  bool isEmpty() const {
    if (matches.size() == 0) return true;
    return false;
  }

  AmbiguousType getType() const;

  ContextSearchResult(std::vector<std::shared_ptr<Decl>> l,   std::vector<std::shared_ptr<Decl>> m)
    : localMatches(l), matches(m) {}
};

#endif
