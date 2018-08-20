#include <gtest/gtest.h>

#include "AST/Type.h"

TEST(ListType, ListType) {
  const Type* t1 = ListType::getInstance(BooleanType::getInstance(), 5);
  const Type* t2 = ListType::getInstance(BooleanType::getInstance(), 5);
  const Type* t3 = ListType::getInstance(BooleanType::getInstance(), 6);
  ASSERT_EQ(t1, t2);
  ASSERT_NE(t1, t3);
  ASSERT_FALSE(t1->isIntegerType());
  ASSERT_FALSE(t1->isDoubleType());
  ASSERT_FALSE(t1->isBooleanType());
}

TEST(ListType, getKind) {
  const Type* t1 = ListType::getInstance(BooleanType::getInstance(), 5);
  ASSERT_EQ(t1->getKind(), Type::Kind::ListType);
}

TEST(ListType, getCanonicalType) {
  const Type* t1 = ListType::getInstance(BooleanType::getInstance(), 5);
  ASSERT_EQ(t1->getCanonicalType(), t1);
}

TEST(ListType, toString) {
  const Type* t1 = ListType::getInstance(BooleanType::getInstance(), 5);
  ASSERT_EQ(t1->toString(), "[Boolean, 5]");
}
