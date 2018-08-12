#include "Sema/BuiltinDecl.h"
#include "AST/Decl.h"
#include "AST/Type.h"
#include <memory>

FuncDecl BuiltinDecl::add_int{
  Token{"+", Token::operator_id, -1, -1, -1}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, IntegerType::getInstance()
, nullptr
};

FuncDecl BuiltinDecl::assign_int{
  Token{"=", Token::operator_id, -1, -1, -1}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, IntegerType::getInstance()
, nullptr
};

FuncDecl BuiltinDecl::equ_int{
  Token{"==", Token::operator_id, -1, -1, -1}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, BooleanType::getInstance()
, nullptr
};
