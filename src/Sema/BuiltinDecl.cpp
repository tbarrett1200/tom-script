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

BasicDecl BuiltinDecl::div_int{
  Token{Token::operator_id, StringRef{"/"}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , IntegerType::getInstance()
  )
};

BasicDecl BuiltinDecl::mod_int{
  Token{Token::operator_id, StringRef{"%"}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , IntegerType::getInstance()
  )
};


BasicDecl BuiltinDecl::ne_int{
  Token{Token::operator_id, StringRef{"!="}}
, FunctionType::getInstance(
    {IntegerType::getInstance(), IntegerType::getInstance()}
  , BooleanType::getInstance()
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



BasicDecl BuiltinDecl::add_dbl{
  Token{Token::operator_id, StringRef{"+"}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , DoubleType::getInstance()
  )
};

BasicDecl BuiltinDecl::sub_dbl{
  Token{Token::operator_id, StringRef{"-"}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , DoubleType::getInstance()
  )
};

BasicDecl BuiltinDecl::mul_dbl{
  Token{Token::operator_id, StringRef{"*"}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , DoubleType::getInstance()
  )
};

BasicDecl BuiltinDecl::div_dbl{
  Token{Token::operator_id, StringRef{"/"}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , DoubleType::getInstance()
  )
};

BasicDecl BuiltinDecl::mod_dbl{
  Token{Token::operator_id, StringRef{"%"}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , DoubleType::getInstance()
  )
};


BasicDecl BuiltinDecl::ne_dbl{
  Token{Token::operator_id, StringRef{"!="}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , BooleanType::getInstance()
  )
};

BasicDecl BuiltinDecl::equ_dbl{
  Token{Token::operator_id, StringRef{"=="}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , BooleanType::getInstance()
  )
};

BasicDecl BuiltinDecl::lt_dbl{
  Token{Token::operator_id, StringRef{"<"}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , BooleanType::getInstance()
  )
};

BasicDecl BuiltinDecl::gt_dbl{
  Token{Token::operator_id, StringRef{">"}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , BooleanType::getInstance()
  )
};

BasicDecl BuiltinDecl::gte_dbl{
  Token{Token::operator_id, StringRef{">="}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , BooleanType::getInstance()
  )
};

BasicDecl BuiltinDecl::lte_dbl{
  Token{Token::operator_id, StringRef{"<="}}
, FunctionType::getInstance(
    {DoubleType::getInstance(), DoubleType::getInstance()}
  , BooleanType::getInstance()
  )
};
