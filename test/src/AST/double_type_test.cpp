#include <gtest/gtest.h>

#include "AST/Type.h"

TEST(DoubleType, DoubleType) {
  const Type* t1 = DoubleType::getInstance();
  const Type* t2 = DoubleType::getInstance();
  ASSERT_EQ(t1, t2);
  ASSERT_FALSE(t1->isIntegerType());
  ASSERT_TRUE(t1->isDoubleType());
  ASSERT_FALSE(t1->isBooleanType());
}

TEST(DoubleType, getKind) {
  const Type* t1 = DoubleType::getInstance();
  ASSERT_EQ(t1->getKind(), Type::Kind::DoubleType);
}

TEST(DoubleType, getCanonicalType) {
  const Type* t1 = DoubleType::getInstance();
  ASSERT_EQ(t1->getCanonicalType(), t1);
}

TEST(DoubleType, toString) {
  const Type* t1 = DoubleType::getInstance();
  ASSERT_EQ(t1->toString(), "f64");
}
