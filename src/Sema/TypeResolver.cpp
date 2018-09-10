#include "AST/Type.h"
#include "AST/Decl.h"
#include "AST/DeclContext.h"

#include "Sema/TypeResolver.h"

void TypeResolver::resolve(class Type &type) {
  switch(type.getKind()) {
  // simple types are already resolved
  case Type::Kind::CharacterType:
  case Type::Kind::IntegerType:
  case Type::Kind::DoubleType:
  case Type::Kind::BooleanType: break;

  // type identifiers must have their qualified-type looked up in the current
  // context and set
  case Type::Kind::TypeIdentifier:
    resolve(static_cast<TypeIdentifier&>(type));
    break;
  // compound types may contain type-identifiers, so they must be recursively
  // walked and resolved.
  case Type::Kind::ReferenceType:
    resolve(static_cast<ReferenceType&>(type));
    break;

  case Type::Kind::PointerType:
    resolve(static_cast<PointerType&>(type));
    break;

  case Type::Kind::SliceType:
    resolve(static_cast<SliceType&>(type));
    break;

  case Type::Kind::StructType:
    resolve(static_cast<StructType&>(type));
    break;

  case Type::Kind::TupleType:
    resolve(static_cast<TupleType&>(type));
    break;

  case Type::Kind::FunctionType:
    resolve(static_cast<FunctionType&>(type));
    break;

  case Type::Kind::ListType:
    resolve(static_cast<ListType&>(type));
    break;

  default: throw std::logic_error("not implemented: resolution of " + type.toString());
  }
}

void TypeResolver::resolve(class TypeIdentifier& type) {
  if (Decl *type_decl = context_.getDecl(StringRef{type.name().data(), static_cast<int>(type.name().length())})) {
    switch(type_decl->getKind()) {

    case Decl::Kind::TypeAlias:
      type.setCanonicalType(static_cast<TypeAlias*>(type_decl)->getType()->getCanonicalType());
      break;

    case Decl::Kind::StructDecl:
      type.setCanonicalType(static_cast<StructDecl*>(type_decl)->getType()->getCanonicalType());
      break;

    default: throw CompilerException(nullptr, "identifier is not a type: " + type.name());
    }
  } else throw CompilerException(nullptr, "unable to resolve type identifier: " + type.name());
}

void TypeResolver::resolve(class PointerType& type) {
  resolve(*type.getReferencedType());
  type.setCanonicalType(
    PointerType::getInstance(type.getReferencedType()->getCanonicalType())
  );
}

void TypeResolver::resolve(class ReferenceType& type) {
  resolve(*type.getReferencedType());
  type.setCanonicalType(
    ReferenceType::getInstance(type.getReferencedType()->getCanonicalType())
  );
}

void TypeResolver::resolve(class SliceType& type) {
  resolve(*type.element());
  type.setCanonicalType(
    SliceType::getInstance(type.element()->getCanonicalType())
  );
}

void TypeResolver::resolve(class StructType& type) {
  std::vector<Type*> canonical_members;
  for (auto member_type: type.elements()) {
    resolve(*member_type);
    canonical_members.push_back(member_type->getCanonicalType());
  }
}

void TypeResolver::resolve(class TupleType& type) {
  for (auto member_type: type.elements()) {
    resolve(*member_type);
  }
}

void TypeResolver::resolve(class FunctionType& type) {
  Type* canonical_return;
  std::vector<Type*> canonical_params;

  resolve(*type.getReturnType());
  canonical_return = type.getReturnType()->getCanonicalType();

  for (auto param_type: type.getParamTypes()) {
    resolve(*param_type);
    canonical_params.push_back(param_type->getCanonicalType());
  }

  type.setCanonicalType(FunctionType::getInstance(canonical_params, canonical_return));
}

void TypeResolver::resolve(class ListType& type) {
  resolve(*type.element_type());
}

void TypeResolver::resolve(class MapType& type) {
  resolve(*type.getKeyType());
  resolve(*type.getValueType());
}
