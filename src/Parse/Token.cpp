#include "Parse/Token.h"
#include <vector>
#include <algorithm>
#include <iostream>

Token::Token(std::string lexeme, int type, int loc, int row, int col) {
  this->lexeme = lexeme;
  this->type = type;
  this->loc = loc;
  this->row = row;
  this->col = col;
}

Token::Token() {
  lexeme = "";
  type = unknown;
  loc = 0;
  row = 0;
  col = 0;
}

int Token::getType() {
  return type;
}

bool Token::is(int t) {
  return type == t;
}

bool Token::isNot(int t) {
  return type != t;
}

bool Token::isAny(std::vector<int> types) {
  for(auto t: types) {
    if (type == t) {
      return true;
    }
  }
  return false;
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
  return os << t.lexeme << " r:"  << t.row << " c:" << t.col;
}
