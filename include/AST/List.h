#ifndef AST_LIST_H
#define AST_LIST_H

using namespace std;

#include <memory>
#include <stdexcept>

template <typename Type>
class List {
public:
  unique_ptr<Type> element;
  unique_ptr<List<Type>> list;
  List(unique_ptr<Type> e, unique_ptr<List<Type>> l)
  : element{move(e)}, list{move(l)} {
    if (!element) {
      throw std::domain_error("expr list: element is required");
    }
  }
  int size() {
    if (list != nullptr) {
      return 1 + list->size();
    } else return 1;
  }
  template <typename T> bool has() {
    if (list != nullptr) {
      return dynamic_cast<T*>(element.get()) != nullptr || list->template has<T>();
    } else return dynamic_cast<T*>(element.get()) != nullptr;
  }
};


#endif
