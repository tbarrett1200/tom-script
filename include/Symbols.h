#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <vector>
#include <map>
#include <string>

class SymbolTable {
public:
  std::map<std::string, std::string> symbols;
  std::vector<std::string> types;
  std::shared_ptr<SymbolTable> parent;
  SymbolTable() = default;
  void defineParent(std::shared_ptr<SymbolTable>);
  void registerSymbol(std::string, std::string);
  bool hasSymbol(std::string, bool);
  std::string getType(std::string symbol);
  void registerType(std::string);
  bool hasType(std::string, bool);
};


#endif
