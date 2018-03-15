#include "AST/AmbiguousType.h"
#include "AST/Type.h"
bool AmbiguousType::isEmpty() const {
  return types.size() == 0;
}
bool AmbiguousType::isAmbiguous() const {
  return types.size() > 1;
}
std::shared_ptr<Type> AmbiguousType::get() const {
  return types[0];
}

AmbiguousType AmbiguousType::filterFunction(std::shared_ptr<TypeList> list) const {
  std::vector<std::shared_ptr<Type>> filtered;
  for (auto type: types) {
    auto func = dynamic_pointer_cast<FunctionType>(type);
    if (func && !list && !func->params) {
      filtered.push_back(type);
    } else if (func && list && func->params && list->matches(static_cast<Matchable&>(*func->params))) {
      filtered.push_back(type);
    }
  }
  return {filtered};
}

bool AmbiguousType::has(std::shared_ptr<Type> t) const {
  for (auto type: types) {
    if (type->matches(dynamic_cast<Matchable&>(*t))) return true;
  } return false;
}

std::ostream& operator<<(std::ostream& os, const AmbiguousType& t) {
  os << t.types.size() << " possible types" << std::endl;
  for (auto type: t.types) {
    os << "\t" << type << std::endl;
  }
  return os;
}
