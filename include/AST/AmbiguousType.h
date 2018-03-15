#ifndef AST_AMBIGUOUS_TYPE_H
#define AST_AMBIGUOUS_TYPE_H

#include <vector>
#include <memory>
#include <iostream>

class Type;
class TypeList;

class AmbiguousType {
public:
  std::vector<std::shared_ptr<Type>> types;
  bool isEmpty() const;
  bool isAmbiguous() const;
  bool has(std::shared_ptr<Type>) const;
  std::shared_ptr<Type> get() const;
  AmbiguousType filterFunction(std::shared_ptr<TypeList>) const;
  AmbiguousType(std::vector<std::shared_ptr<Type>> t) : types{t} {}
};

std::ostream& operator<<(std::ostream& os, const AmbiguousType& t);

#endif
