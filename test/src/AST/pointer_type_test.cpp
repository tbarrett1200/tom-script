#include <gtest/gtest.h>

#include "AST/Type.h"

TEST(PointerType, PointerType) {
  const Type* ref_t1 = IntegerType::getInstance();
  const Type* ref_t2 = IntegerType::getInstance();
  const Type* ref_t3 = DoubleType::getInstance();
  const PointerType* t1 = PointerType::getInstance(ref_t1);
  const PointerType* t2 = PointerType::getInstance(ref_t2);
  const PointerType* t3 = PointerType::getInstance(ref_t3);
  ASSERT_EQ(t1, t2);
  ASSERT_NE(t1, t3);
  ASSERT_NE(t2, t3);
  ASSERT_TRUE(t1->getReferencedType()->isIntegerType());
  ASSERT_TRUE(t2->getReferencedType()->isIntegerType());
  ASSERT_TRUE(t3->getReferencedType()->isDoubleType());
}

TEST(PointerType, getKind) {
  const Type* t1 = PointerType::getInstance(IntegerType::getInstance());
  ASSERT_EQ(t1->getKind(), Type::Kind::PointerType);
}

TEST(PointerType, getCanonicalType) {
  const Type* t1 = PointerType::getInstance(IntegerType::getInstance());
  ASSERT_EQ(t1->getCanonicalType(), t1);
}

TEST(PointerType, toString) {
  const PointerType* t1 = PointerType::getInstance(IntegerType::getInstance());
  ASSERT_EQ(t1->toString(), "*" + t1->getReferencedType()->toString());
}
