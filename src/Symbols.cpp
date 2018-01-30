#include "Symbols.h"
#include <algorithm>
#include <vector>
#include <string>

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> p) : parent{p} {}

void SymbolTable::addSymbol(std::string s, std::string t) {
  symbols.push_back(s);
  types.push_back(t);
}

bool SymbolTable::hasSymbol(std::string s, bool recursive) {
  if (std::find(symbols.begin(), symbols.end(), s) != symbols.end()) {
    return true;
  } else if (recursive && parent != nullptr) {
    return parent->hasSymbol(s);
  } else return false;
}

std::string SymbolTable::getType(std::string s) {
  if (hasSymbol(s, false)) {
    ptrdiff_t diff = std::find(symbols.begin(), symbols.end(), s) - symbols.begin();
    return types[diff];
  } else if (parent->hasSymbol(s)) {
    return parent->getType(s);
  } else throw -1;
}
