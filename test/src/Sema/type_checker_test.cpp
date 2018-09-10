#include <memory>

#include <gtest/gtest.h>

#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/DeclContext.h"
#include "Sema/TypeChecker.h"

TEST(TypeChecker, checkIntegerExpr) {
  Token int_token{Token::integer_literal, {"42"}};
  auto int_expr = std::make_unique<IntegerExpr>(int_token);
  auto decl_context = std::make_unique<DeclContext>();
  TypeChecker type_checker{decl_context.get()};

  ASSERT_EQ(int_expr->getType(), nullptr);
  type_checker.checkIntegerExpr(*int_expr);
  ASSERT_EQ(int_expr->getType(), IntegerType::getInstance());
}


TEST(TypeChecker, checkDoubleExpr) {
  Token dbl_token{Token::double_literal, {"3.14159"}};
  auto dbl_expr = std::make_unique<DoubleExpr>(dbl_token);
  auto decl_context = std::make_unique<DeclContext>();
  TypeChecker type_checker{decl_context.get()};

  ASSERT_EQ(dbl_expr->getType(), nullptr);
  type_checker.checkDoubleExpr(*dbl_expr);
  ASSERT_EQ(dbl_expr->getType(), DoubleType::getInstance());
}

TEST(TypeChecker, checkCharacterExpr) {
  Token char_token{Token::character_literal, {"'z'"}};
  auto char_expr = std::make_unique<CharacterExpr>(char_token);
  auto decl_context = std::make_unique<DeclContext>();
  TypeChecker type_checker{decl_context.get()};

  ASSERT_EQ(char_expr->getType(), nullptr);
  type_checker.checkCharacterExpr(*char_expr);
  ASSERT_EQ(char_expr->getType(), CharacterType::getInstance());
}


TEST(TypeChecker, checkBooleanExpr) {
  Token bool_token{Token::kw_true, {"true"}};
  auto bool_expr = std::make_unique<BoolExpr>(bool_token);
  auto decl_context = std::make_unique<DeclContext>();
  TypeChecker type_checker{decl_context.get()};

  ASSERT_EQ(bool_expr->getType(), nullptr);
  type_checker.checkBoolExpr(*bool_expr);
  ASSERT_EQ(bool_expr->getType(), BooleanType::getInstance());
}

TEST(TypeChecker, checkStringExpr) {
  Token string_token{Token::string_literal, {"\"i am groot\""}};
  auto string_expr = std::make_unique<StringExpr>(string_token);
  auto decl_context = std::make_unique<DeclContext>();
  TypeChecker type_checker{decl_context.get()};

  // a string literal should be interpreted as an array of characters with a
  // length equal to the number of characters plus one. We currently use
  // c-style strings, which are terminated by a null character.
  auto expected_type = ListType::getInstance(CharacterType::getInstance(), 11);

  // the string should not be typed upon creation
  ASSERT_EQ(string_expr->getType(), nullptr);

  type_checker.checkStringExpr(*string_expr);

  ASSERT_EQ(string_expr->getType(), expected_type);
}
