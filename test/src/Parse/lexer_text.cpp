#include <gtest/gtest.h>
#include "Parse/Lexer.h"
#include "Basic/Token.h"

static Lexer make_lexer(std::string str) {
  std::stringstream ss{str};
  std::shared_ptr<SourceFile> src = std::make_shared<SourceFile>(ss);
  return Lexer(src);
}

TEST(Lexer, Lexer) {
  ASSERT_NO_THROW(make_lexer("testing"));
}

TEST(Lexer, lexPunctuaction) {
  std::vector<std::string> possible_operators{
    "[", "]", "(", ")", "{", "}", ":", ";", ",", "\\", "\n"
  };

  std::stringstream no_space;
  for (auto &str: possible_operators) {
    no_space << str;
  }

  std::stringstream space;
  for (auto &str: possible_operators) {
    space << " " << str;
  }

  std::stringstream many_space;
  for (auto &str: possible_operators) {
    many_space << " \t \f \v \r" << str;
  }

  Token t;

  Lexer no_space_lexer = make_lexer(no_space.str());
  for (auto &str: possible_operators) {
    t = no_space_lexer.next();
    StringRef str_ref{str.data(), (int)(str.size())};
    ASSERT_EQ(t.lexeme(), str_ref);
  }
  ASSERT_TRUE(no_space_lexer.next().is(Token::eof));

  Lexer space_lexer = make_lexer(space.str());
  for (auto str: possible_operators) {
    t = space_lexer.next();
    StringRef str_ref{str.data(), (int)(str.size())};
    ASSERT_EQ(t.lexeme(), str_ref);
  }
  ASSERT_TRUE(space_lexer.next().is(Token::eof));


  Lexer many_space_lexer = make_lexer(many_space.str());
  for (auto str: possible_operators) {
    t = many_space_lexer.next();
    StringRef str_ref{str.data(), (int)(str.size())};
    ASSERT_EQ(t.lexeme(), str_ref);
  }
  ASSERT_TRUE(many_space_lexer.next().is(Token::eof));

}

TEST(Lexer, lexOperator) {
  std::vector<std::string> possible_operators{
    "=", "+=", "++", "+", "-=", "--", "->", "-", "*=", "*", "/=", "/", "%="
  , "%", "<<=", "<<", "<", ">>=", ">>", ">", "&=", "&&", "&", "|=", "||", "|"
  , "^=", "^", "~", ".", "?", "!"
  };

  std::stringstream no_space;
  for (auto &str: possible_operators) {
    no_space << str;
  }

  std::stringstream space;
  for (auto &str: possible_operators) {
    space << " " << str;
  }

  std::stringstream many_space;
  for (auto &str: possible_operators) {
    many_space << " \t \f \v \r" << str;
  }

  Token t;

  Lexer no_space_lexer = make_lexer(no_space.str());
  for (auto &str: possible_operators) {
    t = no_space_lexer.next();
    ASSERT_TRUE(t.is(Token::operator_id));
    StringRef str_ref{str.data(), (int)(str.size())};
    ASSERT_EQ(t.lexeme(), str_ref);
  }
  ASSERT_TRUE(no_space_lexer.next().is(Token::eof));

  Lexer space_lexer = make_lexer(space.str());
  for (auto str: possible_operators) {
    t = space_lexer.next();
    ASSERT_TRUE(t.is(Token::operator_id));
    StringRef str_ref{str.data(), (int)(str.size())};
    ASSERT_EQ(t.lexeme(), str_ref);
  }
  ASSERT_TRUE(space_lexer.next().is(Token::eof));


  Lexer many_space_lexer = make_lexer(many_space.str());
  for (auto str: possible_operators) {
    t = many_space_lexer.next();
    ASSERT_TRUE(t.is(Token::operator_id));
    StringRef str_ref{str.data(), (int)(str.size())};
    ASSERT_EQ(t.lexeme(), str_ref);
  }
  ASSERT_TRUE(many_space_lexer.next().is(Token::eof));

}


TEST(Lexer, lexInteger) {
  std::vector<std::string> possible_operators{
    "0",  "343435", "1252534534"
  };

  std::stringstream space;
  for (auto &str: possible_operators) {
    space << " " << str;
  }

  Token t;

  Lexer space_lexer = make_lexer(space.str());
  for (auto str: possible_operators) {
    t = space_lexer.next();
    ASSERT_TRUE(t.is(Token::integer_literal));
    StringRef str_ref{str.data(), (int)(str.size())};
    ASSERT_EQ(t.lexeme(), str_ref);
  }
  ASSERT_TRUE(space_lexer.next().is(Token::eof));

}


TEST(Lexer, lexDouble) {
  std::vector<std::string> possible_operators{
    "2.0", "3434.34345", "2343.", "345345e2234", "345345E34", "343.345e234",
    "234.6789E6786"
  };

  std::stringstream space;
  for (auto &str: possible_operators) {
    space << " " << str;
  }

  Token t;

  Lexer space_lexer = make_lexer(space.str());
  for (auto str: possible_operators) {
    t = space_lexer.next();
    ASSERT_TRUE(t.is(Token::double_literal));
    StringRef str_ref{str.data(), (int)(str.size())};
    ASSERT_EQ(t.lexeme(), str_ref);
  }
  ASSERT_TRUE(space_lexer.next().is(Token::eof));

}

TEST(Lexer, lexIdentifier) {
  std::vector<std::string> possible_operators{
    "sdfvsdfvsdf", "mmbet6435", "efd_dfvds34"
  };

  std::stringstream space;
  for (auto &str: possible_operators) {
    space << " " << str;
  }

  Token t;

  Lexer space_lexer = make_lexer(space.str());
  for (auto str: possible_operators) {
    t = space_lexer.next();
    ASSERT_TRUE(t.is(Token::identifier));
    StringRef str_ref{str.data(), (int)(str.size())};
    ASSERT_EQ(t.lexeme(), str_ref);
  }
  ASSERT_TRUE(space_lexer.next().is(Token::eof));

}
