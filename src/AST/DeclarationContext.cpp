#include "AST/DeclarationContext.h"
#include "AST/AmbiguousDecl.h"
#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Type.h"
#include "Parse/Parser.h"
#include "Sema/RuntimeStack.h"

bool DeclarationContext::add(std::shared_ptr<Decl> d) {
  for (auto element: elements) {
    if (element->getName() == d->getName() && equal(element->getType(), d->getType(), this)) {
      return false;
    }
  }
  d->location = ComputedAddress(getSize()+1, Register::BASE_POINTER, Register::NONE, 0);
  elements.push_back(d);
  return true;
}

bool DeclarationContext::add(std::shared_ptr<Decl> d, std::function<ComputedAddress(const DeclarationContext&)> loc) {
  for (auto element: elements) {
    if (element->getName() == d->getName() && equal(element->getType(), d->getType(), this)) {
      return false;
    }
  }
  d->location = loc(*this);
  elements.push_back(d);
  return true;
}


bool DeclarationContext::hasLocal(std::shared_ptr<Decl> d) {
  if (!d) return false;

  for (auto element: elements) {
    if (element->getName() == d->getName()) {
      return true;
    }
  }
  return false;
}

std::shared_ptr<TypeList> DeclarationContext::getFundamentalType(std::shared_ptr<TypeList> t) {
  if (!t) return nullptr;
  auto e = getFundamentalType(t->element);
  auto list = t->list ? getFundamentalType(t->list) : nullptr;
  return std::make_shared<TypeList>(e, list);
}

std::shared_ptr<Type> DeclarationContext::getFundamentalType(std::shared_ptr<Type> t) {
  if (!t) return nullptr;
  if (t->getKind() == Type::Kind::TupleType) {
    return std::make_shared<TupleType>(getFundamentalType(t->as<TupleType>()->list));
  } else if (t->getKind() == Type::Kind::FunctionType) {
      return std::make_shared<FunctionType>(getFundamentalType(t->as<FunctionType>()->params), getFundamentalType(t->as<FunctionType>()->returns));
  } else if (t->getKind() == Type::Kind::LabeledType) {
      return std::make_shared<LabeledType>(t->as<LabeledType>()->label, getFundamentalType(t->as<LabeledType>()->type));
  } else if (t->getKind() == Type::Kind::ListType) {
      return std::make_shared<ListType>(getFundamentalType(t->as<ListType>()->type));
  } else if (t->getKind() == Type::Kind::MapType) {
      return std::make_shared<MapType>(getFundamentalType(t->as<MapType>()->keyType), getFundamentalType(t->as<MapType>()->valType));
  } else {
    return t;
  }
}

int DeclarationContext::getSize() const {
  int size = 0;
  for (auto decl: elements) {
    if (std::dynamic_pointer_cast<VarDecl>(decl) || std::dynamic_pointer_cast<LetDecl>(decl))
      size++;
  }
  return size;
}

bool DeclarationContext::has(std::shared_ptr<Decl> d) {
  return hasLocal(d) || (parent && parent->has(d));
}

AmbiguousDecl DeclarationContext::filter(std::shared_ptr<class IdentifierExpr> e) {
  AmbiguousDecl self = AmbiguousDecl{elements}.filter(e);
  if (self.isEmpty()) {
    return parent ? parent->filter(e) : self;
  } else return self;
}

AmbiguousDecl DeclarationContext::filter(std::function<bool(std::shared_ptr<Decl>)> func) {
  AmbiguousDecl self = AmbiguousDecl{elements}.filter(func);
  if (self.isEmpty()) {
    return parent ? parent->filter(func) : self;
  } else return self;
}
