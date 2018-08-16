#ifndef BUILTIN_DECL_H
#define BUILTIN_DECL_H

#include "AST/DeclContext.h"
#include "AST/Decl.h"
#include <memory>

class BuiltinDecl {
public:
  static BasicDecl add_int;
  static BasicDecl sub_int;
  static BasicDecl mul_int;

  static BasicDecl assign_int;
  static BasicDecl equ_int;
  static BasicDecl lt_int;
  static BasicDecl lte_int;
  static BasicDecl gt_int;
  static BasicDecl gte_int;

};
#endif
