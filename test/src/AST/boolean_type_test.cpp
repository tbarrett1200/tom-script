#include <gtest/gtest.h>

#include "AST/Type.h"

TEST(BooleanType, BooleanType) {
  const Type* t1 = BooleanType::getInstance();
  const Type* t2 = BooleanType::getInstance();
  ASSERT_EQ(t1, t2);
  ASSERT_FALSE(t1->isIntegerType());
  ASSERT_FALSE(t1->isDoubleType());
  ASSERT_TRUE(t1->isBooleanType());
}

TEST(BooleanType, getKind) {
  const Type* t1 = BooleanType::getInstance();
  ASSERT_EQ(t1->getKind(), Type::Kind::BooleanType);
}

TEST(BooleanType, getCanonicalType) {
  const Type* t1 = BooleanType::getInstance();
  ASSERT_EQ(t1->getCanonicalType(), t1);
}

TEST(BooleanType, toString) {
  const Type* t1 = BooleanType::getInstance();
  ASSERT_EQ(t1->toString(), "bool");
}
