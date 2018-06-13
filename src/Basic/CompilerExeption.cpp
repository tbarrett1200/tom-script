#include "Basic/CompilerException.h"


std::ostream& operator<<(std::ostream &os, CompilerExceptionCategory cat) {
  switch (cat) {
  case CompilerExceptionCategory::Info:
    os << "info";
    break;
  case CompilerExceptionCategory::Warning:
    os << "warning";
    break;
  case CompilerExceptionCategory::Error:
    os << "error";
    break;
  }
  return os;
}

std::ostream& operator<<(std::ostream &os, const CompilerException &e) {
  return os << e.file << ":" << e.loc << ": " << e.category << ": " << e.message;;
}
