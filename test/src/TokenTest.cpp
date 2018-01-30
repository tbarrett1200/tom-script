#include <gtest/gtest.h>
#include "Token.h"


TEST(Token, DefaultConstructor) {
  Token tok;
  EXPECT_EQ(tok.lexeme,"");
  EXPECT_EQ(tok.type,Token::unknown);
  EXPECT_EQ(tok.loc,0);
  EXPECT_EQ(tok.row,0);
  EXPECT_EQ(tok.col,0);
}

TEST(Token, ParameterConstructor) {
  Token tok{"Hello", Token::identifier, 1, 2, 3};
  EXPECT_EQ(tok.lexeme,"Hello");
  EXPECT_EQ(tok.type,Token::identifier);
  EXPECT_EQ(tok.loc,1);
  EXPECT_EQ(tok.row,2);
  EXPECT_EQ(tok.col,3);
}

TEST(Token, is) {
  Token tok{"[", Token::l_square, 0, 0, 0};
  EXPECT_TRUE(tok.is(Token::l_square));
  EXPECT_FALSE(tok.is(Token::identifier));
}

TEST(Token, isNot) {
  Token tok{"[", Token::l_square, 0, 0, 0};
  EXPECT_FALSE(tok.isNot(Token::l_square));
  EXPECT_TRUE(tok.isNot(Token::identifier));
}

TEST(Token, isAny) {
  Token tok{"[", Token::l_square, 0, 0, 0};
  EXPECT_TRUE(tok.isAny({Token::l_square, Token::l_brace}));
  EXPECT_TRUE(tok.isAny({Token::l_brace, Token::l_square}));
  EXPECT_FALSE(tok.isAny({Token::r_brace, Token::r_square}));
}

TEST(Token, isIntLiteral) {
  Token tok1{"123", Token::number, 0, 0, 0};
  EXPECT_TRUE(tok1.isIntLiteral());

  Token tok2{"123", Token::identifier, 0, 0, 0};
  EXPECT_FALSE(tok2.isIntLiteral());

  Token tok3{"123.0", Token::number, 0, 0, 0};
  EXPECT_FALSE(tok3.isIntLiteral());
}

TEST(Token, isDoubleLiteral) {
  Token tok1{"123.0", Token::number, 0, 0, 0};
  EXPECT_TRUE(tok1.isDoubleLiteral());

  Token tok2{"123.0", Token::identifier, 0, 0, 0};
  EXPECT_FALSE(tok2.isDoubleLiteral());

  Token tok3{"123", Token::number, 0, 0, 0};
  EXPECT_FALSE(tok3.isDoubleLiteral());
}
