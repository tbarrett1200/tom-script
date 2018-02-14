#include <gtest/gtest.h>
#include "Type.h"

TEST(Builtin, Constructor) {
  Builtin type1{Builtin::Type::int64};
  EXPECT_EQ(type1.type, Builtin::Type::int64);

  Builtin type2{Builtin::Type::float64};
  EXPECT_EQ(type2.type, Builtin::Type::float64);
}

TEST(Builtin, size) {
  Builtin type1{Builtin::Type::int64};
  EXPECT_EQ(type1.size(), 8);

  Builtin type2{Builtin::Type::float64};
  EXPECT_EQ(type2.size(), 8);
}
