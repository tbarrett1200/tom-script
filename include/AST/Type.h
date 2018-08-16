#ifndef AST_TYPE_DECL
#define AST_TYPE_DECL

#include <iostream>
#include <list>

#include "AST/TreeElement.h"
#include "Basic/Token.h"
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
    instances.push_back(std::make_unique<TypeIdentifier>(n));
    return instances.back().get();
  }

  // Constructors
  TypeIdentifier(std::string n) : name_{n} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::TypeIdentifier; }

  std::string toString() const {
    return name_;
  }

};


class TupleType : public Type {
private:
  static std::vector<std::unique_ptr<TupleType>> instances;

public:
  std::vector<Type*> elements;
  static TupleType* getInstance(std::vector<Type*> e) {
    instances.push_back(std::make_unique<TupleType>(std::move(e)));
    return instances.back().get();
  }

  // Constructors
  TupleType(std::vector<Type*> e) : elements{std::move(e)} {}

  std::string toString() const {
    std::string str = "(";
    for (auto element: elements) {
      str += element->toString() + ",";
    }
    if (elements.size() > 0) str = str.substr(0, str.length()-1);
    str += ")";
    return str;
  }

  // Type Overrides
  Type::Kind getKind() const { return Kind::TupleType; }

};

class FunctionType : public Type {
private:
  static std::vector<std::unique_ptr<FunctionType>> instances;

public:
  std::vector<Type*> params;
  Type* returns;
  static FunctionType* getInstance(std::vector<Type*> p, Type *r) {
    instances.push_back(std::make_unique<FunctionType>(p, r));
    return instances.back().get();
  }

  // Constructors
  FunctionType(std::vector<Type*> p, Type *r) : params{std::move(p)}, returns{r} {}

  std::string toString() const {
    std::string str = "(";
    for (auto param: params) {
      str += (param ? param->toString() : "<nullptr>") + ",";
    }
    if (params.size() > 0) str = str.substr(0, str.length()-1);
    str += ") -> " + (returns ?  returns->toString(): "<nullptr>" );
    return str;
  }

  Type* getReturnType() const {
    return returns;
  }

  int getParamCount() {
    return params.size();
  }

  Type* getParam(int index) {
    return params[index];
  }

  const std::vector<Type*>& getParamTypes() const {
    return params;
  }
  // Type Overridess
  Type::Kind getKind() const { return Kind::FunctionType; }

};

class ListType : public Type {
private:
  static std::vector<std::unique_ptr<ListType>> instances;

public:
  Type* type;
  static ListType* getInstance(Type* t) {
    instances.push_back(std::make_unique<ListType>(t));
    return instances.back().get();
  }

  // Constructors
  ListType(Type* t) : type{t} {}

  std::string toString() const {
    return "[" + type->toString() + "]";
  }

  // Type Overrides
  Type::Kind getKind() const { return Kind::ListType; }

};

class MapType : public Type {
private:
  static std::vector<std::unique_ptr<MapType>> instances;
public:
  Type* keyType;
  Type* valType;
  static MapType* getInstance(Type *k, Type *v) {
    instances.push_back(std::make_unique<MapType>(k, v));
    return instances.back().get();
  }
  // Constructors
  MapType(Type *k, Type *v) : keyType{k}, valType{v} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::MapType; }

  const Type& getKeyType() const {
    return *keyType;
  }

  std::string toString() const {
    return "[" + keyType->toString() + ": " + valType->toString() + "]";
  }

  const Type& getValueType() const {
    return *valType;
  }

};

bool equal(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2);

#endif
