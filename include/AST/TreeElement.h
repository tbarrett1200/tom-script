#ifndef TREE_H
#define TREE_H

#include <vector>
#include <algorithm>
#include <initializer_list>
#include <string>
#include "Basic/SourceCode.h"

class TreeElement {
public:
  virtual std::vector<TreeElement*> getChildren() const {
    return {};
  }
};

#endif
