#include "Type.h"

Builtin::Builtin(Type t): type{t} {}

Builtin::Builtin(string s) {
  if (s == "Int") {
    type = Type::int64;
  } else if (s == "Double") {
    type = Type::float64;
  }
}

int Builtin::size() {
  switch(type) {
    case Type::int64:
      return 8;
    case Type::float64:
      return 8;
  }
}
