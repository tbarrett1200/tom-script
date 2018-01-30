#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
  std::string lexeme;
  int type;
  int loc;
  int row;
  int col;
  enum {
    unknown, eof, identifier, l_brace, l_paren, l_square, r_brace, r_paren,
    r_square, comma, semi, colon, backslash, number, string_literal, operator_identifier,
    key_var, key_let, key_func, key_if, key_while, key_return
  };
  Token(std::string lexeme, int type, int loc, int row, int col);
  Token();
  bool is(int type);
  bool isNot(int type);
  bool isAny(std::vector<int> types);
  bool isIntLiteral();
  bool isDoubleLiteral();
  friend std::ostream& operator<<(std::ostream& os, const Token& t);
};

#endif
