#ifndef AST_TYPE_DECL
#define AST_TYPE_DECL

#include <iostream>
#include <list>

#include "Basic/SourceCode.h"

/**
 * A base class for all types.
 *
 */
class Type {
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
  template<typename T> const T* as() const {
    return dynamic_cast<const T*>(this);
  }

  /**
   * Returns the kind of the derived type, which makes it easy to check the
   * actual type at runtime. Although built-in checks such as dynamic_cast
   * enable this without the extra type field, explicitly listing possible types
   * allows for an exhaustive switch statement of all possible derived types.
   */
  virtual Type::Kind getKind() const = 0;

  /**
   * Returns the underlying type - stripped of possible type aliasing.
   * A fundamental type - one that does not alias another type or contain
   * any non-fundamental types - returns itself.
   *
   * TODO: Currently - all types returns this. This should be updated
   */
  virtual const Type* getCanonicalType() const;

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
  static const IntegerType* getInstance();

  /// Return runtime type, which is Type::Kind::IntegerType
  Type::Kind getKind() const override { return Type::Kind::IntegerType; }

  /// Return a string representation of the IntegerType as "Int"
  std::string toString() const override { return "Int"; }
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
  static const CharacterType* getInstance();

  /// Return runtime type, which is Type::Kind::IntegerType
  Type::Kind getKind() const override { return Type::Kind::CharacterType; }

  /// Return a string representation of the IntegerType as "Int"
  std::string toString() const override { return "Character"; }
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
  static const BooleanType* getInstance();

  /// Return runtime type, which is Type::Kind::BooleanType
  Type::Kind getKind() const override { return Type::Kind::BooleanType; }

  /// Return a string representation of the BooleanType as "BooleanType"
  std::string toString() const override { return "Boolean"; }
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
  static const DoubleType* getInstance();

  /// Return runtime type, which is Type::Kind::DoubleType
  Type::Kind getKind() const override { return Kind::DoubleType; }

  /// Return a string representation of the DoubleType as "Double"
  std::string toString() const override { return "Double"; }

};

/// A type which represents a pointer reference to another type
class PointerType: public Type {
private:
  /// Singleton instance of PointerType
  static std::vector<std::unique_ptr<PointerType>> instances;

  const Type *ref_type_;

public:
  /// Construct Pointer type with given reference type
  PointerType(const Type *ref_type): ref_type_{ref_type} {}

  /// Return a pointer to a PointerType instance with the given key and value
  /// types. It is guarenteed that all PointerType with the same key and value
  /// types will have the same address, so that PointerType can be compared by
  /// pointer for equality.
  static const PointerType* getInstance(const Type *ref_type) {
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

  const Type* getReferencedType() const { return ref_type_; }
  /// Return a string representation of the IntegerType as "*<ref_type>"
  std::string toString() const override { return "*" + ref_type_->toString(); }

};


/// A type which represents a pointer reference to another type
class ReferenceType: public Type {
private:
  /// Singleton instance of PointerType
  static std::vector<std::unique_ptr<ReferenceType>> instances;

  const Type *ref_type_;

public:
  /// Construct Pointer type with given reference type
  ReferenceType(const Type *ref_type): ref_type_{ref_type} {}

  /// Return a pointer to a PointerType instance with the given key and value
  /// types. It is guarenteed that all PointerType with the same key and value
  /// types will have the same address, so that PointerType can be compared by
  /// pointer for equality.
  static const ReferenceType* getInstance(const Type *ref_type) {
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

  const Type* getReferencedType() const { return ref_type_; }
  /// Return a string representation of the IntegerType as "*<ref_type>"
  std::string toString() const override { return "&" + ref_type_->toString(); }

};


/// A type which represents an string aliased to another type
class TypeIdentifier : public Type {
private:
  std::string name_;

  const Type *aliased_type_;

  /// Singleton instances of active TypeIdentifier types
  static std::vector<std::unique_ptr<TypeIdentifier>> instances;

public:
  /// Construct TypeIdentifier type with given reference name
  TypeIdentifier(std::string n, const Type* type) : name_{n}, aliased_type_{type} {}

  /// Return a pointer to a TypeIdentifier instance with the given key and value
  /// types. It is guarenteed that all TypeIdentifier with the same key and value
  /// types will have the same address, so that TypeIdentifier can be compared by
  /// pointer for equality.
  static const TypeIdentifier* getInstance(std::string n, const Type *alias) {
    const TypeIdentifier type_id{n, alias};
    auto it = std::find_if(instances.begin(), instances.end(), [&type_id](auto &type){
      return type_id == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<TypeIdentifier>(std::move(type_id)));
      return instances.back().get();
    }
  }

  // Type Overrides
  Type::Kind getKind() const override { return Kind::TypeIdentifier; }

  /// Return runtime type, which is Type::Kind::TypeIdentifier
  bool operator==(const TypeIdentifier &type) const {
    return name_ == type.name_;
  };

  /// Return a string representation of the TypeIdentifier as "<name>"
  std::string toString() const override {
    return name_ ;
  }

};

/// A type which represents a tuple
class TupleType : public Type {
private:
  std::vector<const Type*> elements_;

  /// Singleton instances of active TupleType types
  static std::vector<std::unique_ptr<TupleType>> instances;

public:
  /// Construct TupleType with the given element vector
  TupleType(std::vector<const Type*> e) : elements_{std::move(e)} {}

  /// Return a pointer to a TupleType instance with the given key and value
  /// types. It is guarenteed that all TupleType with the same key and value
  /// types will have the same address, so that TupleType can be compared by
  /// pointer for equality.
  static const TupleType* getInstance(std::vector<const Type*> elements) {
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
  std::vector<const Type*> params_;
  const Type* returns_;

  /// Singleton instances of active FunctionType types
  static std::vector<std::unique_ptr<FunctionType>> instances;

public:

  /// Construct a FunctionType with the given param and return types
  FunctionType(std::vector<const Type*> params, const Type *returns)
  : params_{std::move(params)}, returns_{returns} {}


  /// Return a pointer to a FunctionType instance with the given key and value
  /// types. It is guarenteed that all FunctionType with the same key and value
  /// types will have the same address, so that FunctionType can be compared by
  /// pointer for equality.
  static const FunctionType* getInstance(std::vector<const Type*> params, const Type *returns) {
    const FunctionType func_type{params, returns};
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
  bool operator==(const FunctionType &type) const {
    return params_ == type.params_ && returns_ == type.returns_;
  };

  /// Return the runtime type, which is Type::Kind::FunctionType
  Type::Kind getKind() const override { return Kind::FunctionType; }

  /// Return the number of params
  int getParamCount() const {
    return params_.size();
  }

  /// Return a const reference to the param type vector
  const std::vector<const Type*>& getParamTypes() const {
    return params_;
  }

  /// Return the param type at the given index
  const Type* getParam(int index) const {
    return params_[index];
  }

  /// Return a const pointer to the return type
  const Type* getReturnType() const {
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

/// A type which represents a list of contiguous elemenets. Lists are currently
/// not implemented.
class ListType : public Type {
private:
  const Type* element_type_;
  const int size_;

  /// Singleton instances of active List types
  static std::vector<std::unique_ptr<ListType>> instances;

public:

  /// Constructs a ListType with the given element type
  ListType(const Type* element_type, int size) : element_type_{element_type}, size_{size} {}

  /// Return a pointer to a ListType instance with the given key and value types.
  /// It is guarenteed that all ListType with the same key and value types will
  /// have the same address, so that ListType can be compared by pointer for
  /// equality.
  static const ListType* getInstance(const Type* type, int size) {
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
  const Type* element_type() const { return element_type_; }

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
  const Type* key_;
  const Type* val_;

  /// Singleton instances of active Map types
  static std::vector<std::unique_ptr<MapType>> instances;

public:

  /// Constructs a MapType with the given key and value types
  MapType(const Type *key, const Type *val): key_{key}, val_{val} {}

  /// Return a pointer to a MapType instance with the given key and value types.
  /// It is guarenteed that all MapTypes with the same key and value types will
  /// have the same address, so that MapTypes can be compared by pointer for
  /// equality.
  static const MapType* getInstance(const Type *key, const Type *val) {
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
  const Type* getKeyType() const { return key_; }

  /// Return a const pointer to the value type
  const Type* getValueType() const { return val_; }

  /// Return a string representation of the MapType as "[<key>: <val>]"
  std::string toString() const override {
    return "[" + key_->toString() + ": " + key_->toString() + "]";
  }
};

#endif
