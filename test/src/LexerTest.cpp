#include <gtest/gtest.h>
#include "Lexer.h"
#include "ErrorReporter.h"

TEST(Lexer, Constructor) {
  ErrorReporter::mute = true;

  SourceCode source{std::stringstream{"Hello"}, "test"};
  EXPECT_NO_THROW(Lexer{&source});
}

TEST(Lexer, lexKeywords) {
  SourceCode source{std::stringstream{"var let func if while return"}, "test"};
  Lexer lexer{&source};
  EXPECT_TRUE(lexer.next().is(Token::key_var));
  EXPECT_TRUE(lexer.next().is(Token::key_let));
  EXPECT_TRUE(lexer.next().is(Token::key_func));
  EXPECT_TRUE(lexer.next().is(Token::key_if));
  EXPECT_TRUE(lexer.next().is(Token::key_while));
  EXPECT_TRUE(lexer.next().is(Token::key_return));
}

TEST(Lexer, lexStringLiteral) {
  SourceCode source{std::stringstream{"\"Hello\" \"A \\\"Quote\\\"\""}, "test"};
  Lexer lexer{&source};

  Token t1 = lexer.next();
  EXPECT_TRUE(t1.is(Token::string_literal));
  EXPECT_EQ(t1.lexeme, "\"Hello\"");

  Token t2 = lexer.next();
  EXPECT_TRUE(t2.is(Token::string_literal));
  EXPECT_EQ(t2.lexeme, "\"A \\\"Quote\\\"\"");
}

TEST(Lexer, lexNumber) {
  SourceCode source{std::stringstream{"10 10. 10.0 10e4 10E4 10.0e4 123fff"}, "test"};
  Lexer lexer{&source};

  Token t = lexer.next();
  EXPECT_TRUE(t.is(Token::number));
  EXPECT_EQ(t.lexeme, "10");

  Token t1 = lexer.next();
  EXPECT_TRUE(t1.is(Token::number));
  EXPECT_EQ(t1.lexeme, "10.");

  Token t2 = lexer.next();
  EXPECT_TRUE(t2.is(Token::number));
  EXPECT_EQ(t2.lexeme, "10.0");

  Token t3 = lexer.next();
  EXPECT_TRUE(t3.is(Token::number));
  EXPECT_EQ(t3.lexeme, "10e4");

  Token t4 = lexer.next();
  EXPECT_TRUE(t4.is(Token::number));
  EXPECT_EQ(t4.lexeme, "10E4");

  Token t5 = lexer.next();
  EXPECT_TRUE(t5.is(Token::number));
  EXPECT_EQ(t5.lexeme, "10.0e4");

  Token t6 = lexer.next();
  EXPECT_TRUE(t6.is(Token::unknown));
  EXPECT_EQ(t6.lexeme, "123fff");
}

TEST(Lexer, lexIdentifier) {
  SourceCode source{std::stringstream{"abc a1 a_1 a"}, "test"};
  Lexer lexer{&source};

  Token t1 = lexer.next();
  EXPECT_TRUE(t1.is(Token::identifier));
  EXPECT_EQ(t1.lexeme, "abc");

  Token t2 = lexer.next();
  EXPECT_TRUE(t2.is(Token::identifier));
  EXPECT_EQ(t2.lexeme, "a1");

  Token t3 = lexer.next();
  EXPECT_TRUE(t3.is(Token::identifier));
  EXPECT_EQ(t3.lexeme, "a_1");

  Token t4 = lexer.next();
  EXPECT_TRUE(t4.is(Token::identifier));
  EXPECT_EQ(t4.lexeme, "a");
}

TEST(Lexer, lexPunctuation) {
  SourceCode source{std::stringstream{"[](){},;:\\"}, "test"};
  Lexer lexer{&source};

  EXPECT_TRUE(lexer.next().is(Token::l_square));
  EXPECT_TRUE(lexer.next().is(Token::r_square));
  EXPECT_TRUE(lexer.next().is(Token::l_paren));
  EXPECT_TRUE(lexer.next().is(Token::r_paren));
  EXPECT_TRUE(lexer.next().is(Token::l_brace));
  EXPECT_TRUE(lexer.next().is(Token::r_brace));
  EXPECT_TRUE(lexer.next().is(Token::comma));
  EXPECT_TRUE(lexer.next().is(Token::semi));
  EXPECT_TRUE(lexer.next().is(Token::colon));
  EXPECT_TRUE(lexer.next().is(Token::backslash));
}

TEST(Lexer, lexEOF) {
  SourceCode source{std::stringstream{"123"}, "test"};
  Lexer lexer{&source};

  lexer.next();
  EXPECT_TRUE(lexer.next().is(Token::eof));
  EXPECT_TRUE(lexer.next().is(Token::eof));
  EXPECT_TRUE(lexer.next().is(Token::eof));
  EXPECT_TRUE(lexer.next().is(Token::eof));
}

/*
unknown, eof,  operator_identifier,
*/
