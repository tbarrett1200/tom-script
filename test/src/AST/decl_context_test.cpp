#include <gtest/gtest.h>
#include "AST/DeclContext.h"
#include "AST/Type.h"
#include "AST/Decl.h"

TEST(DeclContextKey, operator_lt) {
  DeclContextKey key1{StringRef{"abc"},{}};
  DeclContextKey key2{StringRef{"abc"},{}};
  DeclContextKey key3{StringRef{"def"},{}};
  DeclContextKey key4{StringRef{"abc"},{IntegerType::getInstance()}};
  DeclContextKey key5{StringRef{"abc"},{IntegerType::getInstance()}};
  DeclContextKey key6{StringRef{"+"},{IntegerType::getInstance(), IntegerType::getInstance()}};
  DeclContextKey key7{StringRef{"+"},{DoubleType::getInstance(), DoubleType::getInstance()}};

  // strings are lexographically equal... therefore 1 < 2 && 2 < 1
  ASSERT_FALSE(key1 < key2);
  ASSERT_FALSE(key2 < key1);

  // strings are lexographically less... therefore 1 < 3 && 2 < 3
  ASSERT_TRUE(key1 < key3);
  ASSERT_TRUE(key2 < key3);

  // strings are lexographically equal, but have less params
  ASSERT_TRUE(key1 < key4);
  ASSERT_TRUE(key2 < key4);

  // strings are lexographically less
  ASSERT_TRUE(key4 < key3);

  ASSERT_TRUE(key6 < key7 || key7 < key6);
}

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

  ASSERT_TRUE(context.getDeclMap().find({StringRef{"+"}, {
    IntegerType::getInstance(), IntegerType::getInstance()}
  }) != context.getDeclMap().end());

}
