#include <gtest/gtest.h>

#include "AST/Type.h"

TEST(IntegerType, IntegerType) {
  const Type* t1 = IntegerType::getInstance();
  const Type* t2 = IntegerType::getInstance();
  ASSERT_EQ(t1, t2);
  ASSERT_TRUE(t1->isIntegerType());
  ASSERT_FALSE(t1->isDoubleType());
  ASSERT_FALSE(t1->isBooleanType());
}

TEST(IntegerType, getKind) {
  const Type* t1 = IntegerType::getInstance();
  ASSERT_EQ(t1->getKind(), Type::Kind::IntegerType);
}

TEST(IntegerType, getCanonicalType) {
  const Type* t1 = IntegerType::getInstance();
  ASSERT_EQ(t1->getCanonicalType(), t1);
}

TEST(IntegerType, toString) {
  const Type* t1 = IntegerType::getInstance();
  ASSERT_EQ(t1->toString(), "i64");
}
