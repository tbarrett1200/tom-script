#ifndef AST_TYPE_DECL
#define AST_TYPE_DECL

#include <iostream>

#include "AST/ASTNode.h"
#include "AST/Matchable.h"
#include "AST/Expr.h"
#include "Parse/Token.h"

using namespace std;

class Type : virtual public Matchable {
public:

};


class TypeLabel : public Terminal {
public:
  Token token;

  TypeLabel(Token n)
    : token{n} {}

  std::string getLexeme() const {
    return token.lexeme;
  }
};


class TypeList : public NonTerminal {
public:
  unique_ptr<Type> element;
  unique_ptr<TypeList> list;

  TypeList(unique_ptr<Type> e, unique_ptr<TypeList> l)
    : element{move(e)}, list{move(l)} {}

  std::vector<Matchable*> getChildren() const {
    if (!list) return {element.get()};
    else {
      auto children = list->getChildren();
      children.insert(children.begin(), element.get());
      return children;
    }
  }

  template <typename T> bool has() {
    if (list == nullptr) return true;
    else if (!dynamic_cast<T*>(element.get())) return false;
    else return list->has<T>();
  };
};



class LabeledType : public Type, public NonTerminal {
public:
  unique_ptr<TypeLabel> label;
  unique_ptr<Type> type;

  LabeledType(unique_ptr<TypeLabel> p, unique_ptr<Type> t)
    : label{move(p)}, type{move(t)} {}

  std::vector<Matchable*> getChildren() const {
    return {label.get(), type.get()};
  }
};



class TypeIdentifier : public Type, public Terminal {
public:
  Token token;


  TypeIdentifier(Token n)
    : token{n} {}

  std::string getLexeme() const {
    return token.lexeme;
  }
};


class TupleType : public Type, public NonTerminal {
public:
  unique_ptr<TypeList> list;


  std::vector<Matchable*> getChildren() const {
    return {list.get()};
  }

  TupleType(unique_ptr<TypeList> l)
    : list{move(l)} {}
};



class FunctionType : public Type, public NonTerminal {
public:
  unique_ptr<TypeList> params;
  unique_ptr<Type> returns;


  FunctionType(unique_ptr<TypeList> p, unique_ptr<Type> r)
    : params{move(p)}, returns{move(r)} {}

  std::vector<Matchable*> getChildren() const {
    return {params.get(), returns.get()};
  }
};



class ListType : public Type, public NonTerminal {
public:
  std::unique_ptr<Type> type;


  ListType(unique_ptr<Type> t)
    : type{move(t)} {}

  std::vector<Matchable*> getChildren() const {
    return {type.get()};
  }
};



class MapType : public Type, public NonTerminal {
public:
  unique_ptr<Type> keyType;
  unique_ptr<Type> valType;


  MapType(unique_ptr<Type> k, unique_ptr<Type> v)
    : keyType{move(k)}, valType{move(v)} {}

  std::vector<Matchable*> getChildren() const {
    return {keyType.get(), valType.get()};
  }
};


#endif
