#include "AST/Type.h"
#include "AST/Decl.h"
#include "AST/DeclContext.h"

#include "Sema/TypeResolver.h"

// this function checks the type-kind of the given type, and dispatches it to
// the proper type resolution method. This function contains a switch
// statement without a default clause. Thus, it should throw a warning if
// a type is left out. This provides a safety check for future development. if
// any new type kinds are added, a warning will be raised if resolution of the
// type kind is not handled
void TypeResolver::resolve(class Type &type) {
  switch(type.getKind()) {

  // simple types are already resolved, their canonical types are simply
  // themselves
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
  case Type::Kind::MapType:
    resolve(static_cast<MapType&>(type));
    break;
  }
}

void TypeResolver::resolve(class TypeIdentifier& type) {
  if (Decl *type_decl = context_.getDecl(StringRef{type.name().data(), static_cast<int>(type.name().length())})) {
    switch(type_decl->getKind()) {

    case Decl::Kind::TypeAlias:
      type.setCanonicalType(static_cast<TypeAlias*>(type_decl)->getType()->getCanonicalType());
      break;

    // should a struct declaration simply be a special case of type alias ?
    case Decl::Kind::StructDecl:
      type.setCanonicalType(static_cast<StructDecl*>(type_decl)->getType()->getCanonicalType());
      break;

    default:
      std::stringstream ss;
      ss << "unreachable: the type " << type.name() << " is not able to be resolved ";
      ss << "to a type alias or struct declaration";
      throw CompilerException(nullptr, ss.str());

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

// a struct type is an instance equality based type. Two structs can have the
// exact same data members, but not be equal. Thus, a structs' canonical type
// is simply itself.
void TypeResolver::resolve(class StructType& type) {
  for (auto member_type: type.elements()) {
    resolve(*member_type);
  }
}

void TypeResolver::resolve(class TupleType& type) {
  std::vector<Type*> canonical_members;
  for (auto member_type: type.elements()) {
    resolve(*member_type);
    canonical_members.push_back(member_type->getCanonicalType());
  }
  type.setCanonicalType(
    TupleType::getInstance(canonical_members)
  );
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

  type.setCanonicalType(
    FunctionType::getInstance(canonical_params, canonical_return)
  );
}

void TypeResolver::resolve(class ListType& type) {
  resolve(*type.element_type());
  type.setCanonicalType(
    ListType::getInstance(type.element_type()->getCanonicalType(), type.size())
  );
}

void TypeResolver::resolve(class MapType& type) {
  resolve(*type.getKeyType());
  resolve(*type.getValueType());
  type.setCanonicalType(
    MapType::getInstance(
      type.getKeyType()->getCanonicalType()
    , type.getValueType()->getCanonicalType()
    )
  );
}
