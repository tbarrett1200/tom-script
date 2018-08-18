#ifndef TREE_H
#define TREE_H

#include <vector>

#include "Basic/SourceCode.h"

/// The base class for all tree nodes. All tree elements have a name, a set of
/// atrributes, and a list of children. The methods provided by TreeElement are
/// used for walking the tree as well as converting it to a serialized form
/// such as XML or JSON.
class TreeElement {
public:
  /// Return the name of the TreeElement kind
  virtual std::string name() const = 0;

  /// Return the attributes of the tree element. e.g. type, value etc.
  virtual std::vector<std::pair<std::string, std::string>> getAttributes() const {
    return {};
  }

  /// Return the child tree nodes
  virtual std::vector<TreeElement*> getChildren() const {
    return {};
  }
};

#endif
