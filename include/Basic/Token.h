#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "Basic/SourceCode.h"

class Token {
public:
  std::string lexeme; // the actual text contained in the token

  int type;  // the token type, being one of the enumeration items listed below

  int loc;   // the location of the start of the token, in characters from the
             // start of file
  int row;   // the row number of the start of the token
  int col;   // the column number of the start of the token
  enum {
    unknown, eof, identifier, l_brace, l_paren, l_square, r_brace, r_paren,
    r_square, comma, semi, colon, backslash, integer_literal, double_literal, string_literal, operator_id,
    kw_var, kw_let, kw_func, kw_typedef, kw_if, kw_else,  kw_true, kw_false, kw_while, kw_return, kw_typealias, new_line
  };
  Token(std::string lexeme, int type, int loc, int row, int col);
  Token();
  int getType();
  bool is(int type);
  bool isNot(int type);
  SourceLocation getLocation() const;
  bool isAny(std::vector<int> types);
  friend std::ostream& operator<<(std::ostream& os, const Token& t);
};

#endif
