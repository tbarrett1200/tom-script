#include "AST/DeclContext.h"
#include "AST/ContextSearchResult.h"
#include "AST/Decl.h"

bool DeclContext::hasDecl(const Matchable* pattern, bool recursive = true) {
  for (int i = 0; i < decls.size(); i++) {
    if (decls[i]->matches(*pattern)) return true;
  }

  if (recursive && parent) return parent->hasDecl(pattern, true);
  else return false;
}

bool DeclContext::hasDecl(std::string n, bool recursive = true) {
  for (int i = 0; i < decls.size(); i++) {
    if (decls[i]->getName() == n) return true;
  }

  if (recursive && parent) return parent->hasDecl(n, true);
  else return false;
}

ContextSearchResult DeclContext::getDecl(std::string n, bool recursive) {
  std::vector<shared_ptr<Decl>> local_matches;
  for (auto i = decls.begin(); i != decls.end(); i++)
    if ((*i)->getName() == n)
      local_matches.push_back(*i);
  std::vector<shared_ptr<Decl>> matches = local_matches;
  if (recursive && parent) {
    auto parent_matches = parent->getDecl(n, true).matches;
    matches.insert(matches.end(), parent_matches.begin(), parent_matches.end());
  }
  return ContextSearchResult(local_matches, matches);
}

ContextSearchResult DeclContext::getDecl(Matchable* pattern, bool recursive) {
  std::vector<shared_ptr<Decl>> local_matches;
  if (hasDecl(pattern, false)) {
    for (auto i = decls.begin(); i != decls.end(); i++)
      if ((*i)->matches(static_cast<Matchable&>(*pattern)))
        local_matches.push_back(*i);
  }
  std::vector<shared_ptr<Decl>> matches = local_matches;
  if (recursive && parent && parent->hasDecl(pattern, true)) {
    auto parent_matches = parent->getDecl(pattern, true).matches;
    matches.insert(matches.end(), parent_matches.begin(), parent_matches.end());
  }
  return ContextSearchResult(local_matches, matches);
}
