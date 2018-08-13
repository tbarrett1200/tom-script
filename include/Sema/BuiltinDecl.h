#ifndef BUILTIN_DECL_H
#define BUILTIN_DECL_H

#include "AST/DeclContext.h"
#include "AST/Decl.h"
#include <memory>

class BuiltinDecl {
public:
  static FuncDecl add_int;
  static FuncDecl sub_int;
  static FuncDecl mul_int;

  static FuncDecl assign_int;
  static FuncDecl equ_int;
  static FuncDecl lt_int;
  static FuncDecl lte_int;
  static FuncDecl gt_int;
  static FuncDecl gte_int;

};
#endif
