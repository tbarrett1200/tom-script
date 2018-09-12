#ifndef AST_TYPE_DECL
#define AST_TYPE_DECL

#include <iostream>
#include <list>
#include <map>

#include "Basic/SourceCode.h"

/**
 * A base class for all types.
 *
 */
class Type {
private:
  Type* canonical_type_ = nullptr;

public:

  /** All possible derivative classes of Type */
  enum class Kind {
    #define TYPE(SELF, PARENT) SELF,
    #include "AST/Type.def"
    #undef TYPE
  };

  virtual ~Type() = default;

  /**
   * A convenience method for easy casting from a generic Type to
   * any of its possible derived types. Returns null if unable to cast.
   */
  template<typename T>  T* as() {
    return dynamic_cast< T*>(this);
  }

  template<typename T> bool is() const {
    return (dynamic_cast<const T*>(this)!=nullptr);
  }

  void setCanonicalType(Type *type) {
    canonical_type_ = type;
  }


  virtual Type* getCanonicalType() {
    return canonical_type_ ?  canonical_type_: this;
  }

  virtual const Type* getCanonicalType() const {
    return canonical_type_ ?  canonical_type_: this;
  }

  /**
   * Returns the kind of the derived type, which makes it easy to check the
   * actual type at runtime. Although built-in checks such as dynamic_cast
   * enable this without the extra type field, explicitly listing possible types
   * allows for an exhaustive switch statement of all possible derived types.
   */
  virtual Type::Kind getKind() const = 0;

  template<typename T> bool cononical_is() {
    return getCanonicalType()->is<T>();
  }

  template<typename T> T* canonical_as() {
    return getCanonicalType()->as<T>();
  }

  /**
   * Returns the underlying type - stripped of possible type aliasing.
   * A fundamental type - one that does not alias another type or contain
   * any non-fundamental types - returns itself.
   *
   * TODO: Currently - all types returns this. This should be updated
 */

  /**
   * Returns whether or not this type is a builtin 'Integer' type. This is done
   * by checking the 'Kind' of the canonical type. A built in integer will
   * have a 'IntegerType' kind.
   */
  bool isIntegerType() const;

  /**
   * Returns whether or not this type is a builtin 'Double' type. This is done
   * by checking the 'Kind' of the canonical type. A built in double will
   * have a 'DoubleType' kind.
   */
  bool isDoubleType() const;

  bool isBooleanType() const;

  virtual std::string toString() const = 0;
};

/// A type which represents an integer
class IntegerType: public Type {
  /// Singleton instance of IntegerType
  static IntegerType singleton;

  /// Construct IntegerType type
  IntegerType() = default;
public:

  /// Return the singleton instance of IntegerType. Type equality can be done
  /// by pointer comparison becase all IntegerType have the same pointer.
  static IntegerType* getInstance();

  /// Return runtime type, which is Type::Kind::IntegerType
  Type::Kind getKind() const override { return Type::Kind::IntegerType; }

  /// Return a string representation of the IntegerType as "Int"
  std::string toString() const override { return "i64"; }
};



/// A type which represents an integer
class CharacterType: public Type {
  /// Singleton instance of IntegerType
  static CharacterType singleton;

  /// Construct IntegerType type
  CharacterType() = default;
public:

  /// Return the singleton instance of IntegerType. Type equality can be done
  /// by pointer comparison becase all IntegerType have the same pointer.
  static CharacterType* getInstance();

  /// Return runtime type, which is Type::Kind::IntegerType
  Type::Kind getKind() const override { return Type::Kind::CharacterType; }

  /// Return a string representation of the IntegerType as "Int"
  std::string toString() const override { return "char"; }
};


/// A type which represents a boolean
class BooleanType: public Type {
private:
  /// Singleton instance of BooleanType
  static BooleanType singleton;

  /// Construct Boolean type
  BooleanType() = default;

public:

  /// Return the singleton instance of BooleanType. Type equality can be done
  /// by pointer comparison becase all BooleanType have the same pointer.
  static BooleanType* getInstance();

  /// Return runtime type, which is Type::Kind::BooleanType
  Type::Kind getKind() const override { return Type::Kind::BooleanType; }

  /// Return a string representation of the BooleanType as "BooleanType"
  std::string toString() const override { return "bool"; }
};

/// A type which represents a double
class DoubleType: public Type {
private:
  /// Singleton instance of DoubleType
  static DoubleType singleton;

  /// Construct Double type
  DoubleType() = default;
public:

  /// Return the singleton instance of DoubleType. Type equality can be done
  /// by pointer comparison becase all DoubleType have the same pointer.
  static DoubleType* getInstance();

  /// Return runtime type, which is Type::Kind::DoubleType
  Type::Kind getKind() const override { return Kind::DoubleType; }

  /// Return a string representation of the DoubleType as "Double"
  std::string toString() const override { return "f64"; }

};

/// A type which represents a pointer reference to another type
class PointerType: public Type {
private:
  /// Singleton instance of PointerType
  static std::vector<std::unique_ptr<PointerType>> instances;

  Type *ref_type_;

public:
  /// Construct Pointer type with given reference type
  PointerType(Type *ref_type): ref_type_{ref_type} {}

  /// Return a pointer to a PointerType instance with the given key and value
  /// types. It is guarenteed that all PointerType with the same key and value
  /// types will have the same address, so that PointerType can be compared by
  /// pointer for equality.
  static PointerType* getInstance(Type *ref_type) {
    const PointerType pointer_type{ref_type};
    auto it = std::find_if(instances.begin(), instances.end()
    , [&pointer_type](auto &type){
      return pointer_type == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<PointerType>(std::move(pointer_type)));
      return instances.back().get();
    }
  }

  /// Compares fields for equality. This should only be necessary when
  /// constructing a new instance. Otherwise, TupleType should be compared for
  /// pointer equality.
  bool operator==(const PointerType &type) const {
    return ref_type_ == type.ref_type_;
  };

  /// Return runtime type, which is Type::Kind::PointerType
  Type::Kind getKind() const override { return Kind::PointerType; }

  Type* getReferencedType() const { return ref_type_; }
  /// Return a string representation of the IntegerType as "*<ref_type>"
  std::string toString() const override { return "*" + ref_type_->toString(); }

};


/// A type which represents a pointer reference to another type
class ReferenceType: public Type {
private:
  /// Singleton instance of PointerType
  static std::vector<std::unique_ptr<ReferenceType>> instances;

  Type *ref_type_;

public:
  /// Construct Pointer type with given reference type
  ReferenceType(Type *ref_type): ref_type_{ref_type} {}

  /// Return a pointer to a PointerType instance with the given key and value
  /// types. It is guarenteed that all PointerType with the same key and value
  /// types will have the same address, so that PointerType can be compared by
  /// pointer for equality.
  static ReferenceType* getInstance(Type *ref_type) {
    const ReferenceType pointer_type{ref_type};
    auto it = std::find_if(instances.begin(), instances.end()
    , [&pointer_type](auto &type){
      return pointer_type == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<ReferenceType>(std::move(pointer_type)));
      return instances.back().get();
    }
  }

  /// Compares fields for equality. This should only be necessary when
  /// constructing a new instance. Otherwise, TupleType should be compared for
  /// pointer equality.
  bool operator==(const ReferenceType &type) const {
    return ref_type_ == type.ref_type_;
  };

  /// Return runtime type, which is Type::Kind::PointerType
  Type::Kind getKind() const override { return Kind::ReferenceType; }

  Type* getReferencedType() const { return ref_type_; }
  /// Return a string representation of the IntegerType as "*<ref_type>"
  std::string toString() const override { return "&" + ref_type_->toString(); }

};


/// A type which represents an string aliased to another type
class TypeIdentifier : public Type {
private:
  std::string name_;

  /// Singleton instances of active TypeIdentifier types
  static std::vector<std::unique_ptr<TypeIdentifier>> instances;

public:
  /// Construct TypeIdentifier type with given reference name
  TypeIdentifier(std::string n) : name_{n} {}

  /// Return a pointer to a TypeIdentifier instance with the given key and value
  /// types. It is guarenteed that all TypeIdentifier with the same key and value
  /// types will have the same address, so that TypeIdentifier can be compared by
  /// pointer for equality.
  static TypeIdentifier* getInstance(std::string n) {
    TypeIdentifier type_id{n};
    instances.push_back(std::make_unique<TypeIdentifier>(std::move(type_id)));
    return instances.back().get();
  }

  // Type Overrides
  Type::Kind getKind() const override { return Kind::TypeIdentifier; }

  /// Return runtime type, which is Type::Kind::TypeIdentifier
  bool operator==(TypeIdentifier &type) const {
    return name_ == type.name_;
  };


  std::string name() const {
    return name_;
  }
  /// Return a string representation of the TypeIdentifier as "<name>"
  std::string toString() const override {
    return name_ ;
  }

};

/// A type which represents a tuple
class SliceType : public Type {
private:
  Type* element_;

  /// Singleton instances of active TupleType types
  static std::vector<std::unique_ptr<SliceType>> instances;

public:
  /// Construct TupleType with the given element vector
  SliceType(Type* e) : element_{e} {}

  Type* element() const {
      return element_;
  }

  /// Return a pointer to a TupleType instance with the given key and value
  /// types. It is guarenteed that all TupleType with the same key and value
  /// types will have the same address, so that TupleType can be compared by
  /// pointer for equality.
  static SliceType* getInstance(Type *element) {
    const SliceType slice_type{element};
    auto it = std::find_if(instances.begin(), instances.end(), [&slice_type](auto &type){
      return slice_type == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<SliceType>(element));
      return instances.back().get();
    }
  }

  /// Compares fields for equality. This should only be necessary when
  /// constructing a new instance. Otherwise, TupleType should be compared for
  /// pointer equality.
  bool operator==(const SliceType &type) const {
    return element_ == type.element_;
  };

  /// Return runtime type, which is Type::Kind::TupleType
  Type::Kind getKind() const override { return Kind::SliceType; }

  /// Return a string representation of the TupleType as "(<elements>)"
  std::string toString() const override {
    return "&[" + element_->toString() + "]";
  }

};


/// A type which represents a tuple
class TupleType : public Type {
private:
  std::vector<Type*> elements_;

  /// Singleton instances of active TupleType types
  static std::vector<std::unique_ptr<TupleType>> instances;

public:
  /// Construct TupleType with the given element vector
  TupleType(std::vector<Type*> e) : elements_{std::move(e)} {}

  const std::vector<Type*>& elements() const {
      return elements_;
  }

  /// Return a pointer to a TupleType instance with the given key and value
  /// types. It is guarenteed that all TupleType with the same key and value
  /// types will have the same address, so that TupleType can be compared by
  /// pointer for equality.
  static TupleType* getInstance(std::vector<Type*> elements) {
    const TupleType tuple_type{elements};
    auto it = std::find_if(instances.begin(), instances.end(), [&tuple_type](auto &type){
      return tuple_type == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<TupleType>(std::move(tuple_type)));
      return instances.back().get();
    }
  }

  /// Compares fields for equality. This should only be necessary when
  /// constructing a new instance. Otherwise, TupleType should be compared for
  /// pointer equality.
  bool operator==(const TupleType &type) const {
    return elements_ == type.elements_;
  };

  /// Return runtime type, which is Type::Kind::TupleType
  Type::Kind getKind() const override { return Kind::TupleType; }

  /// Return a string representation of the TupleType as "(<elements>)"
  std::string toString() const override {
    std::string str = "(";
    for (auto element: elements_) {
      str += element->toString() + ",";
    }
    if (elements_.size() > 0) str = str.substr(0, str.length()-1);
    str += ")";
    return str;
  }

};

/// A type which represents a functional mapping type
class FunctionType : public Type {
private:
  std::vector<Type*> params_;
  Type* returns_;
  bool vararg_;

  /// Singleton instances of active FunctionType types
  static std::vector<std::unique_ptr<FunctionType>> instances;

public:

  /// Construct a FunctionType with the given param and return types
  FunctionType(std::vector<Type*> params, Type *returns, bool vararg=false)
  : params_{std::move(params)}, returns_{returns}, vararg_{vararg} {}


  /// Return a pointer to a FunctionType instance with the given key and value
  /// types. It is guarenteed that all FunctionType with the same key and value
  /// types will have the same address, so that FunctionType can be compared by
  /// pointer for equality.
  static FunctionType* getInstance(std::vector<Type*> params, Type *returns, bool vararg=false) {
    FunctionType func_type{params, returns, vararg};
    auto it = std::find_if(instances.begin(), instances.end(), [&func_type](auto &type){
      return func_type == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<FunctionType>(std::move(func_type)));
      return instances.back().get();
    }
  }

  /// Compares fields for equality. This should only be necessary when
  /// constructing a new instance. Otherwise, FunctionType should be compared
  /// for pointer equality.
  bool operator==(FunctionType &type) const {
    return params_ == type.params_ && returns_ == type.returns_ && vararg_ == type.vararg_;
  };

  bool isVarArg() const {
    return vararg_;
  }

  /// Return the runtime type, which is Type::Kind::FunctionType
  Type::Kind getKind() const override { return Kind::FunctionType; }

  /// Return the number of params
  int getParamCount() const {
    return params_.size();
  }

  /// Return a const reference to the param type vector
  const std::vector<Type*>& getParamTypes() const {
    return params_;
  }

  /// Return the param type at the given index
  Type* getParam(int index) const {
    return params_[index];
  }

  /// Return a const pointer to the return type
  Type* getReturnType() const {
    return returns_;
  }

  /// Return a string representation of the FunctionType as
  /// "(<params>) -> <returns>"
  std::string toString() const override {
    std::string str = "(";
    for (auto param: params_) {
      str += param->toString() + ", ";
    }
    if (params_.size() > 0) str = str.substr(0, str.length() - 1);
    str += ") -> " + (returns_ ? returns_->toString(): "()" );
    return str;
  }
};

/*
 * A type which represents a structured data with named fields and methods.
 * A struct type with a given list of fields is considered different from an
 * struct with the equivelant fields. A struct is meant to be referenced by
 * name.
 */
class StructType: public Type {
private:
  std::vector<std::pair<std::string, Type*>> members_;

  /// Singleton instances of active List types
  static std::vector<std::unique_ptr<StructType>> instances;
public:
  StructType(std::vector<std::pair<std::string, Type*>> members)
  : members_{std::move(members)} {}


  Type::Kind getKind() const override { return Kind::StructType; }

  static StructType* getInstance(std::vector<std::pair<std::string, Type*>> members) {
    instances.push_back(std::make_unique<StructType>(std::move(members)));
    return instances.back().get();
  }

  std::vector<Type*> elements() const {
    std::vector<Type*> elements;
    for (auto it = members_.begin(); it != members_.end(); ++it ) {
        elements.push_back( it->second );
    }
    return elements;
  }

  int index_of(std::string name) const {
    for (auto it = members_.begin(); it != members_.end(); it++) {
      if (it->first == name) {
        return std::distance(members_.begin(), it);
      }
    }
    return -1;
  }

  std::vector<std::pair<std::string, Type*>>::iterator members_begin() {
    return members_.begin();
  }

  std::vector<std::pair<std::string, Type*>>::iterator members_end() {
    return members_.end();
  }

  std::vector<std::pair<std::string, Type*>>::const_iterator members_begin() const {
    return members_.begin();
  }

  std::vector<std::pair<std::string, Type*>>::const_iterator members_end() const {
    return members_.end();
  }

  Type* type_of_member_at(int index) const {
    return members_[index].second;
  }

  /// Return a string representation of the list type as "[<element-type>]"
  std::string toString() const override {
    std::stringstream ss;
    ss << "{" << std::endl;
    for (auto field: members_) {
      ss << "  " << field.first << ": " << field.second->toString() << std::endl;
    }
    ss << "}" << std::endl;
    return ss.str();
  }
};

/// A type which represents a list of contiguous elemenets. Lists are currently
/// not implemented.
class ListType : public Type {
private:
  Type* element_type_;
  const int size_;

  /// Singleton instances of active List types
  static std::vector<std::unique_ptr<ListType>> instances;

public:

  /// Constructs a ListType with the given element type
  ListType(Type* element_type, int size) : element_type_{element_type}, size_{size} {}

  /// Return a pointer to a ListType instance with the given key and value types.
  /// It is guarenteed that all ListType with the same key and value types will
  /// have the same address, so that ListType can be compared by pointer for
  /// equality.
  static ListType* getInstance(Type* type, int size) {
    const ListType list_type{type, size};
    auto it = std::find_if(instances.begin(), instances.end()
    , [&list_type](auto &type){
      return list_type == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<ListType>(std::move(list_type)));
      return instances.back().get();
    }
  }

  /// Compares fields for equality. This should only be necessary when
  /// constructing a new instance. Otherwise, ListTypes should be compared for
  /// pointer equality.
  bool operator==(const ListType &type) const {
    return element_type_ == type.element_type_ && size_ == type.size_;
  };

  /// Return the runtime type of the Type, which is Type::Kind::ListType
  Type::Kind getKind() const override { return Kind::ListType; }

  /// Return a const pointer to the element type
  Type* element_type() const { return element_type_; }

  /// Return a const pointer to the element type
  int size() const { return size_; }

  /// Return a string representation of the list type as "[<element-type>]"
  std::string toString() const override {
    std::stringstream ss;
    ss << "[" << element_type_->toString() << ", " << size_ << "]";
    return ss.str();
  }
};

/// A type which represents a key-value pair mapping. Maps are currently not
/// implemented. It is unlikely that maps will be fully implemented. Rather,
/// they will probably be put off until some sort of template system is in
/// place.
class MapType : public Type {
private:
  Type* key_;
  Type* val_;

  /// Singleton instances of active Map types
  static std::vector<std::unique_ptr<MapType>> instances;

public:

  /// Constructs a MapType with the given key and value types
  MapType(Type *key, Type *val): key_{key}, val_{val} {}

  /// Return a pointer to a MapType instance with the given key and value types.
  /// It is guarenteed that all MapTypes with the same key and value types will
  /// have the same address, so that MapTypes can be compared by pointer for
  /// equality.
  static MapType* getInstance(Type *key, Type *val) {
    MapType map_type{key, val};
    auto it = std::find_if(instances.begin(), instances.end()
    , [&map_type](auto &type){
      return map_type == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<MapType>(std::move(map_type)));
      return instances.back().get();
    }
  }

  /// Compares fields for equality. This should only be necessary when
  /// constructing a new instance. Otherwise, MapTypes should be compared for
  /// pointer equality.
  bool operator==(const MapType &type) const {
    return key_ == type.key_ && val_ == type.val_;
  };

  /// Return the runtime type of the Type
  Type::Kind getKind() const override { return Kind::MapType; }

  /// Return a const pointer to the key type
  Type* getKeyType() const { return key_; }

  /// Return a const pointer to the value type
  Type* getValueType() const { return val_; }

  /// Return a string representation of the MapType as "[<key>: <val>]"
  std::string toString() const override {
    return "[" + key_->toString() + ": " + key_->toString() + "]";
  }
};

#endif
