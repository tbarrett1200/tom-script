#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "Basic/SourceCode.h"

class Token {
private:
  int type_;
  StringRef lexeme_;
public:
  enum {
    unknown, eof, identifier, l_brace, l_paren, l_square, r_brace, r_paren,
    r_square, comma, semi, dot, colon, backslash, integer_literal, double_literal, string_literal, operator_id,
    kw_var, kw_let, kw_func, kw_typedef, kw_if, kw_else, kw_then, kw_true, kw_false, kw_while, kw_return, kw_typealias, new_line
  };
  Token(int type, const char *loc, int length): type_{type}, lexeme_{loc, length} {}
  Token(int type, StringRef str): type_{type}, lexeme_{str} {}
  Token(): type_{unknown}, lexeme_{nullptr, 0} {}

  StringRef lexeme() const {
    return lexeme_;
  }

  const char* location() const {
    return lexeme_.start;
  }

  int length() const {
    return lexeme_.length;
  }

  int type() {
    return type_;
  }

  bool is(int t) {
    return type_ == t;
  }

  bool isNot(int t) {
    return type_ != t;
  }

  bool isAny(std::vector<int> types) {
    for(auto t: types) {
      if (type_ == t) {
        return true;
      }
    }
    return false;
  }

};

#endif
