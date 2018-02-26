/*#include <algorithm>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

bool ValueSymbol::isValueSymbol() {
  return true;
}

bool ValueSymbol::isTypeSymbol() {
  return false;
}

ValueSymbol::ValueSymbol(const std::string& n, const std::shared_ptr<Type>& t) {
  name = n;
  type = t;
}

bool TypeSymbol::isValueSymbol() {
  return false;
}

bool TypeSymbol::isTypeSymbol() {
  return true;
}

TypeSymbol::TypeSymbol(const std::string& n, const std::shared_ptr<Type>& t) {
  name = n;
  type = t;
}

bool Scope::isGlobal() {
  return parent ? false : true;
}

void Scope::define(std::shared_ptr<Symbol> sym) {
  std::string name = sym->name;

  if (hasSymbol(name, false)) {
    throw std::invalid_argument(name + " already defined");
  } else if (sym->isValueSymbol()) {
    values.push_back(std::dynamic_pointer_cast<ValueSymbol>(sym));
  }

  symbols[name] = sym;
}

bool Scope::hasSymbol(std::string name, bool recursive) {
  if (symbols.find(name) != symbols.end()) {
    return true;
  } else if (recursive && !isGlobal()) {
    return parent->hasSymbol(name, recursive);
  } else return false;
}
auto Scope::getSymbol(std::string name, bool recursive) -> std::shared_ptr<Symbol> {
  if (symbols.find(name) != symbols.end()) {
    return symbols[name];
  } else if (recursive && !isGlobal()) {
    return parent->getSymbol(name, recursive);
  } else return nullptr;
}

bool Scope::hasValueSymbol(std::string name, bool recursive) {
  return getValueSymbol(name, recursive) != nullptr;
}

auto Scope::getValueSymbol(std::string name, bool recursive) -> std::shared_ptr<ValueSymbol> {
  return std::dynamic_pointer_cast<ValueSymbol>(getSymbol(name, recursive));
}

bool Scope::hasTypeSymbol(std::string name, bool recursive) {
  return getTypeSymbol(name, recursive) != nullptr;
}

auto Scope::getTypeSymbol(std::string name, bool recursive) -> std::shared_ptr<TypeSymbol> {
  return std::dynamic_pointer_cast<TypeSymbol>(getSymbol(name, recursive));
}

void Scope::setParent(std::shared_ptr<Scope> p) {
  parent = p;
}

auto Scope::getLocalValues() -> std::vector<std::shared_ptr<ValueSymbol>> {
  return values;
}
*/
