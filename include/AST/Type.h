#ifndef AST_TYPE_DECL
#define AST_TYPE_DECL

#include <iostream>
#include <list>

#include "AST/ASTNode.h"
#include "AST/Matchable.h"
#include "Parse/Token.h"

using namespace std;

class Type : virtual public Matchable {
public:
  enum class Kind {
    #define TYPE(SELF, PARENT) SELF,
    #include "AST/Type.def"
    #undef TYPE
  };
  virtual Type::Kind getKind() const = 0;
};

ostream& operator<<(ostream& os, Type* x);
class TypeLabel : public Terminal {
public:
  Token token;

  TypeLabel(Token n)
    : token{n} {}

  std::string getLexeme() const {
    return token.lexeme;
  }
};
ostream& operator<<(ostream& os, TypeLabel* x);

class TypeList : public NonTerminal {
public:
  shared_ptr<Type> element;
  shared_ptr<TypeList> list;

  TypeList(std::vector<std::shared_ptr<Type>> l) {
    if (l.size() == 0) {
      throw std::runtime_error("type list must have at least one type");
    }
    if (l.size() == 1) {
      element = l[0];
      list = nullptr;
    } else {
      element = l[0];
      l.erase(l.begin());
      list = std::make_shared<TypeList>(l);
    }
  }

  TypeList(shared_ptr<Type> e, shared_ptr<TypeList> l)
    : element{move(e)}, list{move(l)} {}


  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    if (!list) return {element};
    else {
      auto children = list->getChildren();
      children.insert(children.begin(), element);
      return children;
    }
  }

  int size() const {
    if (!list) return 1;
    else return list->size()+1;
  }

  template <typename T> bool has() {
    if (list == nullptr) return true;
    else if (!dynamic_cast<T*>(element.get())) return false;
    else return list->has<T>();
  };
};

ostream& operator<<(ostream& os, TypeList* x);

class LabeledType : public Type, public NonTerminal {
public:
  shared_ptr<TypeLabel> label;
  shared_ptr<Type> type;

  LabeledType(shared_ptr<TypeLabel> p, shared_ptr<Type> t)
    : label{move(p)}, type{move(t)} {}

  Type::Kind getKind() const { return Kind::LabeledType; }

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {label, type};
  }
};


class TypeIdentifier : public Type, public Terminal {
public:
  Token token;


  TypeIdentifier(Token n)
    : token{n} {}

  Type::Kind getKind() const { return Kind::TypeIdentifier; }


  std::string getLexeme() const {
    return token.lexeme;
  }
};


class TupleType : public Type, public NonTerminal {
public:
  shared_ptr<TypeList> list;

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {list};
  }

  Type::Kind getKind() const { return Kind::TupleType; }

  TupleType(shared_ptr<TypeList> l)
    : list{move(l)} {}

};



class FunctionType : public Type, public NonTerminal {
public:
  shared_ptr<TypeList> params;
  shared_ptr<Type> returns;


  FunctionType(shared_ptr<TypeList> p, shared_ptr<Type> r)
    : params{move(p)}, returns{move(r)} {}

  Type::Kind getKind() const { return Kind::FunctionType; }

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {params, returns};
  }
};

class ListType : public Type, public NonTerminal {
public:
  std::shared_ptr<Type> type;


  ListType(shared_ptr<Type> t)
    : type{move(t)} {}

  Type::Kind getKind() const { return Kind::ListType; }

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {type};
  }
};

class MapType : public Type, public NonTerminal {
public:
  shared_ptr<Type> keyType;
  shared_ptr<Type> valType;


  MapType(shared_ptr<Type> k, shared_ptr<Type> v)
    : keyType{move(k)}, valType{move(v)} {}

  Type::Kind getKind() const { return Kind::MapType; }

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {keyType, valType};
  }
};


#endif
