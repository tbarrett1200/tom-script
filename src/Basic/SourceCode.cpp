#include "Basic/SourceCode.h"

SourceCode* SourceManager::currentSource = nullptr;

std::string SourceManager::currentFile() {
  if (SourceManager::currentSource != nullptr) {
    return currentSource->getPath();
  } else {
    throw new std::logic_error("source does not exist");
  }
}

std::ostream& operator<<(std::ostream &os, SourceLocation loc) {
  return os << loc.row << ":" << loc.col;
}
