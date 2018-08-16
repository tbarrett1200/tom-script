#include <gtest/gtest.h>
#include "Basic/SourceCode.h"

TEST(StringRef, StringRef) {
  ASSERT_NO_THROW(StringRef{"testing"});
  ASSERT_EQ(StringRef{"test"}.length, 4);
}

TEST(StringRef, str) {
  ASSERT_EQ(StringRef{"test"}.str(), "test");
}

TEST(StringRef, operator_equal) {
  ASSERT_EQ(StringRef{"test"}, StringRef{"test"});
  ASSERT_NE(StringRef{"test1"}, StringRef{"test2"});
}

TEST(StringRef, operator_stream) {
  std::stringstream ss;
  ss << StringRef{"test"};
  ASSERT_EQ(ss.str(), "test");
}
