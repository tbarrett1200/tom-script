#include "AST/AmbiguousType.h"
#include "AST/Type.h"

AmbiguousType AmbiguousType::filter(std::function<bool(std::shared_ptr<Type>)> func) const {
  std::vector<std::shared_ptr<Type>> filtered;
  for (auto type: types) {
    if (!func) throw std::logic_error("null function");
    else if (type && func(type)) filtered.push_back(type);
  }
  return {filtered};
}


bool AmbiguousType::contains(std::shared_ptr<Type> t) const {
  for (auto type: types) {
    if (*type == *t) return true;
  }
  return false;
}

std::ostream& operator<<(std::ostream& os, const AmbiguousType& t) {
  if (t.types.size() == 0) {
    os << "undefined" << std::endl;
  } else if (t.types.size() == 1) {
    os << t.get() << std::endl;
  } else {
    os << t.types.size() << " possible types" << std::endl;
    for (auto type: t.types) {
      os << "  " << type << std::endl;
    }
  }

  return os;
}
