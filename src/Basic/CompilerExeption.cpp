#include "Basic/CompilerException.h"


std::ostream& operator<<(std::ostream &os, CompilerExceptionCategory cat) {
  switch (cat) {
  case CompilerExceptionCategory::Info:
    os << "\33[34minfo\033[0m";
    break;
  case CompilerExceptionCategory::Warning:
    os << "\33[33mwarning\033[0m";
    break;
  case CompilerExceptionCategory::Error:
    os << "\33[31merror\033[0m";
    break;
  }
  return os;
}
