#ifndef AST_TYPE_DECL
#define AST_TYPE_DECL

#include <iostream>
#include <list>

#include "AST/Matchable.h"
#include "Parse/Token.h"
#include "Basic/SourceCode.h"

class DeclarationContext;

class Type : virtual public Matchable {
public:
  enum class Kind {
    #define TYPE(SELF, PARENT) SELF,
    #include "AST/Type.def"
    #undef TYPE
  };

  template<typename T> const T* as() const;
  virtual Type::Kind getKind() const = 0;

};

class TypeLabel : public Terminal {
public:
  Token token;

  // Constructors
  TypeLabel(Token n) : token{n} {}

  // Matchable Overrides
  std::string getLexeme() const { return token.lexeme; }
};

class TypeList : public NonTerminal {
public:
  std::shared_ptr<Type> element;
  std::shared_ptr<TypeList> list;

  // Constructors
  TypeList(std::vector<std::shared_ptr<Type>> l);
  TypeList(std::shared_ptr<Type> e, std::shared_ptr<TypeList> l);

  // Matchable Overrides
  std::vector<std::shared_ptr<Matchable>> getChildren() const;

  // Utility Methods
  int size() const;
  std::shared_ptr<Type> operator[] (const int);

  template <typename T> bool has() const {
    if (list == nullptr) return true;
    else if (!dynamic_cast<T*>(element.get())) return false;
    else return list->has<T>();
  };
};


class LabeledType : public Type, public NonTerminal {
public:
  std::shared_ptr<TypeLabel> label;
  std::shared_ptr<Type> type;

  // Constructors
  LabeledType(std::shared_ptr<TypeLabel> p, std::shared_ptr<Type> t);

  // Type Overrides
  Type::Kind getKind() const;

  // Matchable Overrides
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
};


class TypeIdentifier : public Type, public Terminal {
public:
  Token token;

  // Constructors
  TypeIdentifier(Token n) : token{n} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::TypeIdentifier; }

  // Matchable Overrides
  std::string getLexeme() const { return token.lexeme; }
};


class TupleType : public Type, public NonTerminal {
public:
  std::shared_ptr<TypeList> list;

  // Constructors
  TupleType(std::shared_ptr<TypeList> l) : list{move(l)} {}

  // Factory Methods
  static std::shared_ptr<TupleType> make(std::shared_ptr<TypeList>);

  // Type Overrides
  Type::Kind getKind() const { return Kind::TupleType; }

  // Matchable Overrides
  std::vector<std::shared_ptr<Matchable>> getChildren() const { return {list}; }
};

class FunctionType : public Type, public NonTerminal {
public:
  std::shared_ptr<TypeList> params;
  std::shared_ptr<Type> returns;

  // Constructors
  FunctionType(std::shared_ptr<TypeList> p, std::shared_ptr<Type> r) : params{p}, returns{r} {}

  // Type Overridess
  Type::Kind getKind() const { return Kind::FunctionType; }

  // Matchable Overrides
  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {params, returns};
  }
};

class ListType : public Type, public NonTerminal {
public:
  std::shared_ptr<Type> type;

  // Constructors
  ListType(std::shared_ptr<Type> t) : type{t} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::ListType; }

  // Matchable Overrides
  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {type};
  }
};

class MapType : public Type, public NonTerminal {
public:
  std::shared_ptr<Type> keyType;
  std::shared_ptr<Type> valType;

  // Constructors
  MapType(std::shared_ptr<Type> k, std::shared_ptr<Type> v) : keyType{k}, valType{v} {}

  // Type Overrides
  Type::Kind getKind() const { return Kind::MapType; }

  // Matchable Overrides
  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {keyType, valType};
  }
};

std::ostream& operator<<(std::ostream& os, Type* x);
std::ostream& operator<<(std::ostream& os, TypeLabel* x);
std::ostream& operator<<(std::ostream& os, TypeList* x);

bool equal(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2, DeclarationContext *c);
bool equal(std::shared_ptr<TypeList> t1, std::shared_ptr<TypeList> t2, DeclarationContext *c);

bool operator != (const Type& l, const Type& r);
bool operator == (const Type& l, const Type& r);
bool operator == (const TypeList& l, const TypeList& r);

#endif
