#include "Symbols.h"
#include <algorithm>
#include <vector>
#include <string>

void SymbolTable::defineParent(std::shared_ptr<SymbolTable> p) {
  parent = p;
}

void SymbolTable::registerSymbol(std::string s, std::string type) {
  symbols.insert(std::pair<std::string,std::string>(s, type));
}

bool SymbolTable::hasSymbol(std::string s, bool recursive = true) {
  if (symbols.find(s) != symbols.end()) {
    return true;
  } else if (recursive && parent != nullptr) {
    return parent->hasSymbol(s);
  } else return false;
}

std::string SymbolTable::getType(std::string symbol) {
  return symbols[symbol];
}

void SymbolTable::registerType(std::string s) {
  types.push_back(s);
}

bool SymbolTable::hasType(std::string t, bool recursive = true) {
  if (std::find(types.begin(), types.end(), t) != types.end()) {
    return true;
  } else if (recursive && parent != nullptr) {
    return parent->hasType(t);
  } else return false;
}
