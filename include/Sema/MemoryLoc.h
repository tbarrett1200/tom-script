#ifndef SEMA_MEMORY_LOC_H
#define SEMA_MEMORY_LOC_H

#include <ostream>

enum class Register {
  NONE,         /* 0 */
  BASE_POINTER  /* the base of the stack frame for the current function */
};

/* A pure interface for a class which properly accesses the registers */
class RegisterDelegate {
public:
  virtual int* get(Register r) = 0;
};

/* An address in memory located relative to a runtime-determined location */
class ComputedAddress {
  int offset;
  Register base;
  Register index;
  int scale;
public:
  ComputedAddress() = default;
  ComputedAddress(const ComputedAddress&) = default;
  ComputedAddress(ComputedAddress&&) = default;
  ComputedAddress& operator=(const ComputedAddress&) = default;

  ComputedAddress(int o, Register b, Register i, int s);
  static ComputedAddress param(int i);
  int get(RegisterDelegate &delegate);
  friend std::ostream& operator<<(std::ostream& os, const ComputedAddress& a);
};

std::ostream& operator<<(std::ostream& os, const Register& a);

#endif
