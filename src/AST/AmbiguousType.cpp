#include "AST/AmbiguousType.h"
#include "AST/Type.h"
#include "AST/Expr.h"

#include <vector>

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

bool AmbiguousType::contains(std::shared_ptr<Type> t, DeclarationContext* c) const {
  auto f1 = c->getFundamentalType(t);
  for (auto type: types) {
    auto f2 = c->getFundamentalType(type);
    if (*f1 == *f2) return true;
  }
  return false;
}

std::shared_ptr<Type> AmbiguousType::get() const {
  if (isEmpty()) {
    throw std::logic_error("cannot access empty type");
  } else if (isAmbiguous()) {
    throw std::logic_error("cannot access ambiguous type");
  } else {
    return types[0];
  }
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

AmbiguousTypeList::AmbiguousTypeList(AmbiguousType t, AmbiguousTypeList l)
: element{make_shared<AmbiguousType>(t)}, list{make_shared<AmbiguousTypeList>(l)} {}

AmbiguousTypeList::AmbiguousTypeList(AmbiguousType t)
: element{make_shared<AmbiguousType>(t)}, list{nullptr} {}

AmbiguousTypeList::AmbiguousTypeList(std::shared_ptr<ExprList> l)
: element{make_shared<AmbiguousType>(l->element->type)} {
  if (l->list) list = make_shared<AmbiguousTypeList>(l->list);
}


AmbiguousTypeList::AmbiguousTypeList(std::vector<AmbiguousType> l) {
  element = make_shared<AmbiguousType>(l.front());
  l.erase(l.begin());
  list = l.size() > 0 ? make_shared<AmbiguousTypeList>(l) : nullptr;
}

int AmbiguousTypeList::size() const {
  if (!list) return 1;
  else return 1 + list->size();
}

bool AmbiguousTypeList::hasPermutation(std::shared_ptr<TypeList> l) const {
  if (!l) return false;
  if (l->size() != size()) return false;
  else if (!list) return true;
  else return element->contains(l->element) && list->hasPermutation(l->list);
};

bool AmbiguousTypeList::hasPermutation(std::shared_ptr<TypeList> l, DeclarationContext* c) const {
  if (!l) return false;
  if (l->size() != size()) return false;
  else if (!list) return true;
  else return element->contains(l->element, c) && list->hasPermutation(l->list, c);
};

std::vector<TypeList> AmbiguousTypeList::getPermutations() const {
  std::vector<TypeList> result;

  if (list) {
    auto permutations = list->getPermutations();
    for (auto type: element->types)
      for (auto p: permutations)
        result.push_back({type, std::make_shared<TypeList>(p)});
  } else {
    for (auto type: element->types)
      result.push_back({type, nullptr});
  }
  return result;
}

std::ostream& operator<<(std::ostream& os, const AmbiguousTypeList& t) {
  os << *t.element;
  if (t.list) {
    os << *t.list;
  }
  return os;
}
