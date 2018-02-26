/*#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <vector>
#include <map>
#include <string>

#include "Parse/Type.h"

struct Symbol {
  std::string name;
  virtual bool isValueSymbol() = 0;
  virtual bool isTypeSymbol() = 0;
};

struct TypeSymbol : public Symbol {
  std::shared_ptr<Type> type;
  TypeSymbol(const std::string&, const std::shared_ptr<Type>&);
  bool isValueSymbol();
  bool isTypeSymbol();
};

struct ValueSymbol : public Symbol {
  std::shared_ptr<Type> type;
  ValueSymbol(const std::string&, const std::shared_ptr<Type>&);
  bool isValueSymbol();
  bool isTypeSymbol();
};

struct Scope {
private:
  std::map<std::string, std::shared_ptr<Symbol>> symbols;
  std::vector<std::shared_ptr<ValueSymbol>> values;
  std::shared_ptr<Scope> parent;
public:
  Scope() = default;
  bool isGlobal();

  bool hasSymbol(std::string name, bool recursive);
  auto getSymbol(std::string name, bool recursive) -> std::shared_ptr<Symbol>;

  bool hasValueSymbol(std::string name, bool recursive);
  auto getValueSymbol(std::string name, bool recursive) -> std::shared_ptr<ValueSymbol>;

  bool hasTypeSymbol(std::string name, bool recursive);
  auto getTypeSymbol(std::string name, bool recursive) -> std::shared_ptr<TypeSymbol>;

  void define(std::shared_ptr<Symbol> sym);
  void setParent(std::shared_ptr<Scope>);
  auto getLocalValues() -> std::vector<std::shared_ptr<ValueSymbol>>;
};




#endif
*/
