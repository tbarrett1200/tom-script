#ifndef SEMA_TYPE_RESOLVER_H
#define SEMA_TYPE_RESOLVER_H

/*
 * This class perfoms contextual type resolution, which involves the creation
 * of unique 'canonical' type, which can be compared on a pointer by pointer
 * bases for type equality. This involves looking up type identifiers in
 * symbol tables, and creating and setting a unique conanical type based on
 * the retrieved information
 */
class TypeResolver {
private:
  class DeclContext &context_;
public:
  TypeResolver(class DeclContext &context) : context_{context} {};
  void resolve(class Type& type);
  void resolve(class PointerType& type);
  void resolve(class ReferenceType& type);
  void resolve(class SliceType& type);
  void resolve(class StructType& type);
  void resolve(class TypeIdentifier& type);
  void resolve(class TupleType& type);
  void resolve(class FunctionType& type);
  void resolve(class ListType& type);
  void resolve(class MapType& type);
};

#endif
