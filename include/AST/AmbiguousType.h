#ifndef AST_AMBIGUOUS_TYPE_H
#define AST_AMBIGUOUS_TYPE_H

#include <vector>
#include <initializer_list>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <functional>

class Type;

/// A set of possible types of an ambiguous identifier. It can be created from
/// the result of a declaration context search. If the type checking phase is
/// not able to use the surrounding context to narrow down the possibilities to
/// single type, then an error occurs due to unresolved ambiguity. If there are
/// no possibilies left, then an error occurs due to an undefined usage of the
/// identifier. Methods in this class are able to eliminate possibilies in an
/// attempt to find a single type;
class AmbiguousType {
public:
  /// the possible remaining types
  std::vector<std::shared_ptr<Type>> types;

  /// constructs an ambiguous type with the specified vector of types
  AmbiguousType(std::vector<std::shared_ptr<Type>> t) : types{t} {}
  AmbiguousType(std::initializer_list<std::shared_ptr<Type>> t) : types{t} {}

  /// returns true if there are no possible types remaining
  bool isEmpty() const {
    return types.size() == 0;
  }

  /// returns true if there are multiple types remaining
  bool isSingleton() const {
    return types.size() == 1;
  }

  bool isAmbiguous() const {
    return types.size() > 1;
  }

  bool contains(std::shared_ptr<Type>) const;

  /// returns the sole remaining type
  /// throws a std::logic_error if empty or ambiguous
  std::shared_ptr<Type> get() const {
    if (isEmpty()) {
      throw std::logic_error("cannot access empty type");
    } else if (isAmbiguous()) {
      throw std::logic_error("cannot access ambiguous type");
    } else {
      return types[0];
    }
  }

  /// returns an AmbiguousType with only members of specified type subclass
  AmbiguousType filter(std::function<bool(std::shared_ptr<Type>)>) const;

  template <typename T> std::vector<T> map(std::function<T(std::shared_ptr<Type>)> func) const {
    std::vector<T> filtered;
    for (auto type: types) {
      filtered.push_back(func(type));
    }
    return filtered;
  }
};

// prints a human readable representation of the AmbiguousType to the stream
std::ostream& operator<<(std::ostream& os, const AmbiguousType& t);

#endif
