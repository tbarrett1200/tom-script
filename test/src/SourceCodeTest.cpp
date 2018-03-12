#include <gtest/gtest.h>

/*
TEST(SourceCode, Constructor) {
  SourceCode example{std::stringstream{"Testing Testing Testing"}, "test"};
  EXPECT_EQ(example.getText(),"Testing Testing Testing");
  EXPECT_EQ(example.getPath(),"test");
}

TEST(SourceCode, getLineCount) {
  const std::stringstream stream1 {"123"};
  EXPECT_EQ(SourceCode(stream1, "test").getLineCount(),1);

  const std::stringstream stream2 {"123\n456"};
  EXPECT_EQ(SourceCode(stream2, "test").getLineCount(),2);

  const std::stringstream stream3 {"123\n456\n"};
  EXPECT_EQ(SourceCode(stream3, "test").getLineCount(),3);
}

TEST(SourceCode, getLine) {
  std::string text = "123\n456\n";
  EXPECT_EQ(SourceCode(std::stringstream(text), "test").getLine(0),"123");
  EXPECT_EQ(SourceCode(std::stringstream(text), "test").getLine(1),"456");
  EXPECT_EQ(SourceCode(std::stringstream(text), "test").getLine(2),"");

  std::string text2 = "123\n456\nabc";
  EXPECT_EQ(SourceCode(std::stringstream(text2), "test").getLine(2),"abc");
}

TEST(SourceCode, getChar) {
  SourceCode source{std::stringstream{"123"}, "test"};
  EXPECT_EQ(source.getChar(0),'1');
  EXPECT_EQ(source.getChar(1),'2');
  EXPECT_EQ(source.getChar(2),'3');
  EXPECT_EQ(source.getChar(3), -1);
  EXPECT_EQ(source.getChar(50),-1);
}
*/
