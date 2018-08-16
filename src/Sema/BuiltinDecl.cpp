#include "Sema/BuiltinDecl.h"
#include "AST/Decl.h"
#include "AST/Type.h"
#include <memory>

FuncDecl BuiltinDecl::add_int{
  Token{Token::operator_id, StringRef{"+"}}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, IntegerType::getInstance()
, nullptr
};

FuncDecl BuiltinDecl::sub_int{
  Token{Token::operator_id, StringRef{"-"}}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, IntegerType::getInstance()
, nullptr
};

FuncDecl BuiltinDecl::mul_int{
  Token{Token::operator_id, StringRef{"*"}}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, IntegerType::getInstance()
, nullptr
};

FuncDecl BuiltinDecl::assign_int{
  Token{Token::operator_id, StringRef{"="}}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, IntegerType::getInstance()
, nullptr
};

FuncDecl BuiltinDecl::equ_int{
  Token{Token::operator_id, StringRef{"=="}}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, BooleanType::getInstance()
, nullptr
};


FuncDecl BuiltinDecl::lt_int{
  Token{ Token::operator_id,StringRef{"<"}}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, BooleanType::getInstance()
, nullptr
};


FuncDecl BuiltinDecl::gt_int{
  Token{Token::operator_id, StringRef{">"}}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, BooleanType::getInstance()
, nullptr
};

FuncDecl BuiltinDecl::gte_int{
  Token{Token::operator_id, StringRef{">="}}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, BooleanType::getInstance()
, nullptr
};

FuncDecl BuiltinDecl::lte_int{
  Token{Token::operator_id, StringRef{"<="}}
, {std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance()), std::make_shared<ParamDecl>(Token{}, IntegerType::getInstance())}
, BooleanType::getInstance()
, nullptr
};
