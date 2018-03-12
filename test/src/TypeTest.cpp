#include <gtest/gtest.h>
#include "AST/Type.h"
#include "Parse/Parser.h"

TEST(TypeIdentifier, Constructor) {
  const std::stringstream sstream{"Int"};
  auto source = SourceCode{sstream, "test"};
  auto parser = Parser{&source};
  auto type = parser.parseTypeIdentifier();
  ASSERT_NE(type, nullptr);

  EXPECT_TRUE(type->matches(Pattern{"Int"}));
  EXPECT_FALSE(type->matches(Pattern{"Double"}));
}

TEST(TupleType, Constructor) {
  const std::stringstream sstream{"(Int, Double, String)"};
  auto source = SourceCode{sstream, "test"};
  auto parser = Parser{&source};
  auto type = parser.parseTupleType();
  ASSERT_NE(type, nullptr);

  EXPECT_TRUE(type->matches(Pattern{"Int", "Double", "String"}));
  EXPECT_FALSE(type->matches(Pattern{"Int", "String", "String"}));
  EXPECT_FALSE(type->matches(Pattern{"Int", "Double"}));

  EXPECT_TRUE(type->matches(Pattern{"Int", "Double"}, false));
  EXPECT_FALSE(type->matches(Pattern{"Int", "String"}, false));
}
