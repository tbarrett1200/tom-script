#ifndef AST_LIST_H
#define AST_LIST_H

using namespace std;

#include <memory>

template <typename Type>
class List {
  unique_ptr<Type> element;
  unique_ptr<List<Type>> list;
};

#endif
