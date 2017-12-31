#include "Tree.h"

ParseTree::ParseTree(int t, std::vector<ParseTree> c) : type{t} {
  children.insert(children.end(),c.begin(), c.end());
}

ParseTree::ParseTree(int t, Token d) : type{t}, data{d} {}

ParseTree::ParseTree() {}

std::ostream& operator<<(std::ostream& os, const ParseTree& pt) {
  if (pt.children.size() == 0) {
    return os << pt.data.getLexeme();
  }

  if (pt.type == ParseTree::BlockStmt) {
    return os << "{}" << std::endl;
  }

  for (auto child: pt.children) {
    os << child;
  }

  if (pt.type == ParseTree::Stmt) {
    os << std::endl;
  }

  return os;
}
