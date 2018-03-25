#ifndef AST_AMBIGUOUS_DECL_H
#define AST_AMBIGUOUS_DECL_H

#include <vector>
#include <initializer_list>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <functional>

class Decl;
class AmbiguousType;

/// A set of possible decls of an ambiguous identifier. It can be created from
/// the result of a declaration context search. If the decl checking phase is
/// not able to use the surrounding context to narrow down the possibilities to
/// single decl, then an error occurs due to unresolved ambiguity. If there are
/// no possibilies left, then an error occurs due to an undefined usage of the
/// identifier. Methods in this class are able to eliminate possibilies in an
/// attempt to find a single decl;
class AmbiguousDecl {
public:
  std::vector<std::shared_ptr<Decl>> decls;   /// the possible remaining decls

  /// constructs an ambiguous decl with the specified vector of decls
  AmbiguousDecl(std::vector<std::shared_ptr<Decl>> d) : decls{d} {}
  AmbiguousDecl(std::initializer_list<std::shared_ptr<Decl>> d) : decls{d} {}

  /// returns true if there are no possible declarations remaining
  bool isEmpty() const { return decls.size() == 0; }

  /// returns true if there are multiple declarations remaining
  bool isSingleton() const { return decls.size() == 1; }

  /// returns true if there is only one declaration remaining
  bool isAmbiguous() const { return decls.size() > 1; }

  AmbiguousType getTypes() const;

  /// returns the sole remaining declaration or throws a std::logic_error if
  /// the set is empty or ambiguous
  std::shared_ptr<Decl> get() const;

  AmbiguousDecl filter(std::shared_ptr<class IdentifierExpr> e);

  /// returns an AmbiguousType with only members of specified decl subclass
  AmbiguousDecl filter(std::function<bool(std::shared_ptr<Decl>)>) const;
};

/// prints a human readable representation of the AmbiguousType to the stream
std::ostream& operator<<(std::ostream& os, const AmbiguousDecl& t);

#endif
