#ifndef SEMA_MEMORY_LOC_H
#define SEMA_MEMORY_LOC_H

#include <ostream>

/*
 * In order to implement useful features such as recursive function calls, exact
 * memory locations for data can't be determined at runtime. For instance,
 */

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
  /* Default Constructor */
  ComputedAddress() = default;

  /* Default Copy Constructor */
  ComputedAddress(const ComputedAddress&) = default;

  /* Default Assignment.
   * */
  ComputedAddress& operator=(const ComputedAddress&) = default;

  /*
   * Constructs a ComputedAddress with the given members
   * int o       - offset
   * Register b  - base
   * Register i  - index
   * int s       - scale
   */
  ComputedAddress(int o, Register b, Register i, int s);

  /* Factory Method.
   *
   * Returns a ComputedAddress for the nth parameter of a function. This is
   * equivelant to -n(%rbp,0,0). Note that parameters indices start from 1.
   */
  static ComputedAddress makeParam(int n);

  int get(RegisterDelegate& delegate);

  friend std::ostream& operator<<(std::ostream& os, const ComputedAddress& a);
};

std::ostream& operator<<(std::ostream& os, const Register& a);

#endif
