#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <vector>
#include <string>

class SymbolTable {
public:
  std::vector<std::string> symbols;
  std::vector<std::string> types;
  std::shared_ptr<SymbolTable> parent;
  SymbolTable(std::shared_ptr<SymbolTable>);
  void addSymbol(std::string, std::string);
  bool hasSymbol(std::string, bool recursive=true);
  std::string getType(std::string);
};


#endif
