#ifndef BUILTIN_DECL_H
#define BUILTIN_DECL_H

#include "AST/DeclContext.h"
#include "AST/Decl.h"
#include <memory>

class BuiltinDecl {
public:
  static FuncDecl add_int;
  static FuncDecl assign_int;
  static FuncDecl equ_int;

};
#endif
