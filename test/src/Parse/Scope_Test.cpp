#include <gtest/gtest.h>
#include "Parse/Scope.h"
#include "AST/Type.h"

TEST(Scope, addType) {
  ScopeManager m;
  m.addType("abc", IntegerType::getInstance());
  ASSERT_TRUE(m.hasType("abc"));
  ASSERT_FALSE(m.hasType("def"));

  m.push();
  m.addType("abc", DoubleType::getInstance());
  ASSERT_TRUE(m.hasType("abc"));
  ASSERT_TRUE(m.getType("abc")->isDoubleType());

  m.pop();
  ASSERT_TRUE(m.hasType("abc"));
  ASSERT_TRUE(m.getType("abc")->isIntegerType());
}
