#include <gtest/gtest.h>
#include "Basic/SourceCode.h"

TEST(SourceFile, SourceFile) {
  ASSERT_NO_THROW(SourceFile source{"test_data/source_file_test_data.txt"});
}

TEST(SourceFile, line_count) {
  SourceFile source{"test_data/source_file_test_data.txt"};
  ASSERT_EQ(source.line_count(), 4);
}

TEST(SourceFile, path) {
  SourceFile source{"test_data/source_file_test_data.txt"};
  ASSERT_EQ(source.path(), "test_data/source_file_test_data.txt");
}

TEST(SourceFile, line_starts) {
  SourceFile source{"test_data/source_file_test_data.txt"};
  ASSERT_EQ(source.line_starts()[0], 0);
  ASSERT_EQ(source.line_starts()[1], 10);
  ASSERT_EQ(source.line_starts()[2], 20);
  ASSERT_EQ(source.line_starts()[3], 32);
}

TEST(SourceFile, substr) {
  SourceFile source{"test_data/source_file_test_data.txt"};
  ASSERT_EQ(source.substr({0, 0}, {0, 3}), StringRef{"the"});
  ASSERT_EQ(source.substr({0, 0}, {0, 9}), StringRef{"the quick"});
  ASSERT_EQ(source.substr({0, 4}, {0, 9}), StringRef{"quick"});
  ASSERT_EQ(source.substr({0, 4}, {1, 5}), StringRef{"quick\nbrown"});
}

TEST(SourceFile, line) {
  SourceFile source{"test_data/source_file_test_data.txt"};
  ASSERT_EQ(source.line(0), StringRef{"the quick\n"});
  ASSERT_EQ(source.line(1), StringRef{"brown fox\n"});
  ASSERT_EQ(source.line(2), StringRef{"jumped over\n"});
  ASSERT_EQ(source.line(3), StringRef{"the lazy dog\n"});
  ASSERT_THROW(source.line(4), std::out_of_range);
  ASSERT_THROW(source.line(-1), std::out_of_range);
}
