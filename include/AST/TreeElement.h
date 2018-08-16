#ifndef TREE_H
#define TREE_H

#include <vector>

#include "Basic/SourceCode.h"

class TreeElement {
public:
  virtual std::vector<TreeElement*> getChildren() const {
    return {};
  }
};

#endif
