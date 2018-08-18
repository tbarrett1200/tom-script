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
  static BasicDecl div_int;
  static BasicDecl mod_int;
  static BasicDecl neg_int;

  static BasicDecl equ_int;
  static BasicDecl ne_int;
  static BasicDecl lt_int;
  static BasicDecl lte_int;
  static BasicDecl gt_int;
  static BasicDecl gte_int;

  static BasicDecl add_dbl;
  static BasicDecl sub_dbl;
  static BasicDecl mul_dbl;
  static BasicDecl div_dbl;
  static BasicDecl mod_dbl;
  static BasicDecl neg_dbl;

  static BasicDecl equ_dbl;
  static BasicDecl ne_dbl;
  static BasicDecl lt_dbl;
  static BasicDecl lte_dbl;
  static BasicDecl gt_dbl;
  static BasicDecl gte_dbl;

  static BasicDecl int_to_double;
  static BasicDecl double_to_int;
};
#endif
