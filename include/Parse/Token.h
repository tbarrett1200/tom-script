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
    r_square, comma, semi, colon, backslash, number, string_literal, operator_id,
    kw_var, kw_let, kw_func, kw_if, kw_while, kw_return, kw_typealias
  };
  Token(std::string lexeme, int type, int loc, int row, int col);
  Token();
  int getType();
  bool is(int type);
  bool isNot(int type);
  bool isAny(std::vector<int> types);
  bool isIntLiteral();
  bool isDoubleLiteral();
  friend std::ostream& operator<<(std::ostream& os, const Token& t);
};

#endif
