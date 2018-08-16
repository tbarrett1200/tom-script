#ifndef AST_TYPE_DECL
#define AST_TYPE_DECL

#include <iostream>
#include <list>

#include "Basic/Token.h"
#include "Basic/SourceCode.h"

#include "AST/TreeElement.h"

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

/**
 * A builtin
 *
 */
class IntegerType: public Type {
  static IntegerType singleton;

public:

  IntegerType() = default;

  static IntegerType* getInstance();

  /**
   * An Integer's canonical type is always itself.
   */
  const Type* getCanonicalType() const override {
    return this;
  }

  std::string toString() const override {
    return "Int";
  }
  /**
   * Returns Type::Kind::IntegerType
   */
  Type::Kind getKind() const override {
    return Type::Kind::IntegerType;
  }
};

/**
 * A builtin
 *
 */
class PointerType: public Type {
private:
  static PointerType singleton;


public:
  PointerType() = default;

  static PointerType* getInstance();

  /**
   * An Integer's canonical type is always itself.
   */
  const Type* getCanonicalType() const override {
    return this;
  }

  std::string toString() const override {
    return "Pointer";
  }
  /**
   * Returns Type::Kind::IntegerType
   */
  Type::Kind getKind() const override {
    return Type::Kind::PointerType;
  }
};

/**
 * A builtin
 *
 */
class BooleanType: public Type {
private:
  static BooleanType singleton;

public:

  BooleanType() = default;

  BooleanType(const BooleanType&) = delete;
  BooleanType(BooleanType&&) = delete;

  static BooleanType* getInstance();

  std::string toString() const override {
    return "Boolean";
  }
  /**
   * An Integer's canonical type is always itself.
   */
  const Type* getCanonicalType() const override {
    return this;
  }

  /**
   * Returns Type::Kind::IntegerType
   */
  Type::Kind getKind() const override {
    return Type::Kind::BooleanType;
  }
};


/**
 * A builtin
 *
 */
class DoubleType: public Type {
private:
  static DoubleType singleton;
  static std::vector<DoubleType> instances;

public:

  DoubleType() = default;

  DoubleType(const DoubleType&) = delete;
  DoubleType(DoubleType&&) = delete;

  static DoubleType* getInstance();

  /**
   * An Integer's canonical type is always itself.
   */
  const Type* getCanonicalType() const override {
    return this;
  }

  std::string toString() const override {
    return "Double";
  }

  /**
   * Returns Type::Kind::IntegerType
   */
  Type::Kind getKind() const override {
    return Type::Kind::DoubleType;
  }
};


class TypeIdentifier : public Type {
private:
  std::string name_;
  static std::vector<std::unique_ptr<TypeIdentifier>> instances;

public:
  static TypeIdentifier* getInstance(std::string n) {
    const TypeIdentifier type_id{n};
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

  // Constructors
  TypeIdentifier(std::string n) : name_{n} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::TypeIdentifier; }

  bool operator==(const TypeIdentifier &type) const {
    return name_ == type.name_;
  };

  std::string toString() const {
    return name_;
  }

};


class TupleType : public Type {
private:
  static std::vector<std::unique_ptr<TupleType>> instances;
  std::vector<Type*> elements_;

public:
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

  // Constructors
  TupleType(std::vector<Type*> e) : elements_{std::move(e)} {}

  std::string toString() const {
    std::string str = "(";
    for (auto element: elements_) {
      str += element->toString() + ",";
    }
    if (elements_.size() > 0) str = str.substr(0, str.length()-1);
    str += ")";
    return str;
  }
  bool operator==(const TupleType &type) const {
    return elements_ == type.elements_;
  };

  // Type Overrides
  Type::Kind getKind() const { return Kind::TupleType; }

};

class FunctionType : public Type {
private:
  static std::vector<std::unique_ptr<FunctionType>> instances;
  std::vector<Type*> params_;
  Type* returns_;
public:

  static FunctionType* getInstance(std::vector<Type*> params, Type *returns) {
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

  // Constructors
  FunctionType(std::vector<Type*> params, Type *returns)
  : params_{std::move(params)}, returns_{returns} {}

  std::string toString() const {
    std::string str = "(";

    for (auto param: params_) {
      str += param->toString();
    }

    if (params_.size() > 0) str = str.substr(0, str.length()-1);
    str += ") -> " + (returns_ ? returns_->toString(): "()" );
    return str;
  }

  Type* getReturnType() const {
    return returns_;
  }

  bool operator==(const FunctionType &type) const {
    return params_ == type.params_ && returns_ == type.returns_;
  };

  int getParamCount() {
    return params_.size();
  }

  Type* getParam(int index) {
    return params_[index];
  }

  const std::vector<Type*>& getParamTypes() const {
    return params_;
  }
  // Type Overridess
  Type::Kind getKind() const { return Kind::FunctionType; }

};

class ListType : public Type {
private:
  static std::vector<std::unique_ptr<ListType>> instances;
  Type* type_;

public:

  static ListType* getInstance(Type* type) {
    const ListType list_type{type};
    auto it = std::find_if(instances.begin(), instances.end(), [&list_type](auto &type){
      return list_type == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<ListType>(std::move(list_type)));
      return instances.back().get();
    }
  }

  bool operator==(const ListType &type) const {
    return type_ == type.type_;
  };

  // Constructors
  ListType(Type* type) : type_{type} {}

  std::string toString() const {
    return "[" + type_->toString() + "]";
  }

  // Type Overrides
  Type::Kind getKind() const { return Kind::ListType; }

};

class MapType : public Type {
private:
  Type* key_;
  Type* val_;

  static std::vector<std::unique_ptr<MapType>> instances;
public:

  static MapType* getInstance(Type *key, Type *val) {
    MapType map_type{key, val};
    auto it = std::find_if(instances.begin(), instances.end(), [&map_type](auto &type){
      return map_type == *type;
    });
    if (it != instances.end()) {
      return it->get();
    } else {
      instances.push_back(std::make_unique<MapType>(std::move(map_type)));
      return instances.back().get();
    }
  }

  bool operator==(const MapType &type) const {
    return key_ == type.key_ && val_ == type.val_;
  };

  // Constructors
  MapType(Type *key, Type *val): key_{key}, val_{val} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::MapType; }

  const Type& getKeyType() const {
    return *key_;
  }

  std::string toString() const {
    return "[" + key_->toString() + ": " + key_->toString() + "]";
  }

  const Type& getValueType() const {
    return *val_;
  }

};


#endif
