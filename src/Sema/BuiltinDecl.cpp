#include "Sema/BuiltinDecl.h"
#include "AST/Decl.h"
#include "AST/Type.h"
#include <memory>

BasicDecl BuiltinDecl::add_int{
  Token{Token::operator_id, StringRef{"+"}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , IntegerType::getInstance()
  )
};

BasicDecl BuiltinDecl::sub_int{
  Token{Token::operator_id, StringRef{"-"}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , IntegerType::getInstance()
  )
};

BasicDecl BuiltinDecl::mul_int{
  Token{Token::operator_id, StringRef{"*"}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , IntegerType::getInstance()
  )
};

BasicDecl BuiltinDecl::assign_int{
  Token{Token::operator_id, StringRef{"="}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , IntegerType::getInstance()
  )
};

BasicDecl BuiltinDecl::equ_int{
  Token{Token::operator_id, StringRef{"=="}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , BooleanType::getInstance()
  )
};

BasicDecl BuiltinDecl::lt_int{
  Token{Token::operator_id, StringRef{"<"}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , BooleanType::getInstance()
  )
};

BasicDecl BuiltinDecl::gt_int{
  Token{Token::operator_id, StringRef{">"}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , BooleanType::getInstance()
  )
};

BasicDecl BuiltinDecl::gte_int{
  Token{Token::operator_id, StringRef{">="}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , BooleanType::getInstance()
  )
};

BasicDecl BuiltinDecl::lte_int{
  Token{Token::operator_id, StringRef{"<="}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , BooleanType::getInstance()
  )
};
