#include "AST/DeclarationContext.h"
#include "AST/AmbiguousDecl.h"
#include "AST/Decl.h"
#include "AST/Type.h"
#include "Parse/Parser.h"

bool DeclarationContext::add(std::shared_ptr<Decl> d) {
  for (auto element: elements) {
    if (element->getName() == d->getName() && equal(element->getType(), d->getType(), this)) {
      return false;
    }
  }
  elements.push_back(d);
  return true;
}

bool DeclarationContext::hasLocal(std::shared_ptr<Decl> d) {
  for (auto element: elements) {
    if (element->getName() == d->getName() && equal(element->getType(), d->getType(), this)) {
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
    return make_shared<TupleType>(getFundamentalType(t->as<TupleType>()->list));
  } else if (t->getKind() == Type::Kind::FunctionType) {
      return make_shared<FunctionType>(getFundamentalType(t->as<FunctionType>()->params), getFundamentalType(t->as<FunctionType>()->returns));
  } else if (t->getKind() == Type::Kind::LabeledType) {
      return make_shared<LabeledType>(t->as<LabeledType>()->label, getFundamentalType(t->as<LabeledType>()->type));
  } else if (t->getKind() == Type::Kind::ListType) {
      return make_shared<ListType>(getFundamentalType(t->as<ListType>()->type));
  } else if (t->getKind() == Type::Kind::MapType) {
      return make_shared<MapType>(getFundamentalType(t->as<MapType>()->keyType), getFundamentalType(t->as<MapType>()->valType));
  }else if (t->getKind() == Type::Kind::TypeIdentifier) {
    AmbiguousDecl decls = filter([t](std::shared_ptr<Decl> d) {
      return d->as<TypeAlias>() && d->as<TypeAlias>()->getName() == t->as<TypeIdentifier>()->getLexeme();
    });
    if (decls.isSingleton()) {
      return getFundamentalType(decls.get()->getType());
    }
    AmbiguousDecl decls2 = filter([t](std::shared_ptr<Decl> d) {
      return d->as<TypeDecl>() && d->as<TypeDecl>()->getName() == t->as<TypeIdentifier>()->getLexeme();;
    });
    if (decls2.isSingleton()) {
      return Parser::makeType(decls2.get()->getName());
    }
    return t;
  } else {
    return t;
  }
}

bool DeclarationContext::has(std::shared_ptr<Decl> d) {
  return hasLocal(d) || (parent && parent->has(d));
}

AmbiguousDecl DeclarationContext::filter(std::function<bool(std::shared_ptr<Decl>)> func) {
  AmbiguousDecl self = AmbiguousDecl{elements}.filter(func);
  if (self.isEmpty()) {
    return parent ? parent->filter(func) : self;
  } else return self;
}
