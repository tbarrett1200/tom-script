#include "Sema/MemoryLoc.h"
ComputedAddress::ComputedAddress(int o, Register b, Register i, int s)
: offset{o}, base{b}, index{i}, scale{s} {}

ComputedAddress ComputedAddress::makeParam(int i) {
  return ComputedAddress(-i, Register::BASE_POINTER, Register::NONE, 0);
}


int ComputedAddress::get(RegisterDelegate& delegate) {
  int address = offset;
  address += delegate.get(base) ? *delegate.get(base) : 0;
  address += scale * (delegate.get(index) ? *delegate.get(index) : 0);
  return address;
}

std::ostream& operator<<(std::ostream& os, const Register& a) {
  switch(a) {
    case Register::NONE:
      os << "%none";
      break;
    case Register::BASE_POINTER:
      os << "%rbp";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const ComputedAddress& a) {
  if (a.index != Register::NONE && a.base != Register::NONE) {
    os << a.offset << "(" << a.base << ", " << a.index << ", " << a.scale << ")";
  } else if (a.base != Register::NONE) {
    os << a.offset << "(" << a.base << ")";
  } else if (a.index != Register::NONE) {
    os << "illegal address: base required for scaled addresses";
  } else {
    os << a.offset;
  }
    return os;
}
