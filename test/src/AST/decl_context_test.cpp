#include <gtest/gtest.h>
#include "AST/DeclContext.h"
#include "AST/Type.h"
#include "AST/Decl.h"

TEST(DeclContext, addDecl) {
  DeclContext context;
  BasicDecl decl1{Token(Token::identifier,StringRef{"test1"}), IntegerType::getInstance()};
  BasicDecl decl2{Token(Token::identifier,StringRef{"test2"}), DoubleType::getInstance()};
  BasicDecl decl3{Token(Token::identifier,StringRef{"+"}), FunctionType::getInstance({
    IntegerType::getInstance(), IntegerType::getInstance()}
  , IntegerType::getInstance())
  };
  BasicDecl decl4{Token(Token::identifier,StringRef{"+"}), FunctionType::getInstance({
    DoubleType::getInstance(), DoubleType::getInstance()}
  , DoubleType::getInstance())
  };

  context.addDecl(&decl1);
  context.addDecl(&decl2);
  context.addDecl(&decl3);
  context.addDecl(&decl4);

  ASSERT_TRUE(context.getDecl(StringRef{"test1"}));
  ASSERT_TRUE(context.getDecl(StringRef{"test2"}));
  ASSERT_TRUE(context.getDecl({StringRef{"+"}, {IntegerType::getInstance(), IntegerType::getInstance()}}));
  ASSERT_TRUE(context.getDecl({StringRef{"+"}, {DoubleType::getInstance(), DoubleType::getInstance()}}));
  ASSERT_FALSE(context.getDecl({StringRef{"+"}, {BooleanType::getInstance(), BooleanType::getInstance()}}));

}
