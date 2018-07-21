#ifndef AST_TYPE_DECL
#define AST_TYPE_DECL

#include <iostream>
#include <list>

#include "AST/TreeElement.h"
#include "Parse/Token.h"
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
};

/**
 * A builtin
 *
 */
class IntegerType: public Type {
  static std::shared_ptr<IntegerType> singleton;

public:

  IntegerType() = default;

  static std::shared_ptr<IntegerType> getInstance();

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
    return Type::Kind::IntegerType;
  }
};

/**
 * A builtin
 *
 */
class PointerType: public Type {
private:
  static PointerType *singleton;


public:
  PointerType() = default;

  static PointerType* getInstance() {
    if (singleton == nullptr) {
      singleton = new PointerType();
    }
    return singleton;
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
    return Type::Kind::PointerType;
  }
};

/**
 * A builtin
 *
 */
class DoubleType: public Type {
private:
  static std::shared_ptr<DoubleType> singleton;

public:

  DoubleType() = default;

  DoubleType(const DoubleType&) = delete;
  DoubleType(DoubleType&&) = delete;

  static std::shared_ptr<DoubleType> getInstance();

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
    return Type::Kind::DoubleType;
  }
};


class TypeLabel  {
public:
  Token token;

  // Constructors
  TypeLabel(Token n) : token{n} {}

};

class TypeList  {
public:
  std::shared_ptr<Type> element;
  std::shared_ptr<TypeList> list;

  // Constructors
  TypeList(std::vector<std::shared_ptr<Type>> l);
  TypeList(std::shared_ptr<Type> e, std::shared_ptr<TypeList> l);


  // Utility Methods
  int size() const;
  std::shared_ptr<Type> operator[] (const int);

  template <typename T> bool has() const {
    if (list == nullptr) return true;
    else if (!dynamic_cast<T*>(element.get())) return false;
    else return list->has<T>();
  };
};


class LabeledType : public Type {
public:
  std::shared_ptr<TypeLabel> label;
  std::shared_ptr<Type> type;

  // Constructors
  LabeledType(std::shared_ptr<TypeLabel> p, std::shared_ptr<Type> t);

  // Type Overrides
  Type::Kind getKind() const;

};


class TypeIdentifier : public Type {
public:
  Token token;

  // Constructors
  TypeIdentifier(Token n) : token{n} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::TypeIdentifier; }

  /**
   * Returns the name of the type identifier.
   * e.g. Integer
   */
  std::string getName() const;

};


class TupleType : public Type {
public:
  std::shared_ptr<TypeList> list;

  // Constructors
  TupleType(std::shared_ptr<TypeList> l) : list{move(l)} {}

  // Factory Methods
  static std::shared_ptr<TupleType> make(std::shared_ptr<TypeList>);

  // Type Overrides
  Type::Kind getKind() const { return Kind::TupleType; }

};

class FunctionType : public Type {
public:
  std::shared_ptr<TypeList> params;
  std::shared_ptr<Type> returns;

  // Constructors
  FunctionType(std::shared_ptr<TypeList> p, std::shared_ptr<Type> r) : params{p}, returns{r} {}

  // Type Overridess
  Type::Kind getKind() const { return Kind::FunctionType; }

};

class ListType : public Type {
public:
  std::shared_ptr<Type> type;

  // Constructors
  ListType(std::shared_ptr<Type> t) : type{t} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::ListType; }

};

class MapType : public Type {
public:
  std::shared_ptr<Type> keyType;
  std::shared_ptr<Type> valType;

  // Constructors
  MapType(std::shared_ptr<Type> k, std::shared_ptr<Type> v) : keyType{k}, valType{v} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::MapType; }

  const Type& getKeyType() const {
    return *keyType;
  }

  const Type& getValueType() const {
    return *valType;
  }



};

std::ostream& operator<<(std::ostream& os, Type* x);
std::ostream& operator<<(std::ostream& os, TypeLabel* x);
std::ostream& operator<<(std::ostream& os, TypeList* x);

bool equal(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2, class DeclarationContext *c);
bool equal(std::shared_ptr<TypeList> t1, std::shared_ptr<TypeList> t2, class DeclarationContext *c);

bool operator != (const Type& l, const Type& r);
bool operator == (const Type& l, const Type& r);
bool operator == (const TypeList& l, const TypeList& r);

#endif
