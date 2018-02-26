/*#include "CodeGenerator.h"

#include <iostream>

void CodeGenerator::visit(BlockStmt* t) {
  std::vector<std::string> vars;

  for (auto symbol: t->scope->getLocalValues()) {
    std::cout << "# " << symbol->name << " ";
    std::cout << symbol->type << std::endl;
  }

  std::cout << "start:" << std::endl;
}
*/
