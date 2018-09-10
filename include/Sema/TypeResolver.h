#ifndef SEMA_TYPE_RESOLVER_H
#define SEMA_TYPE_RESOLVER_H

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
