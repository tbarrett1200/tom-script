#include "Token.h"
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

Token::Token() {}

bool Token::is(int type) {
  return this->type == type;
}

bool Token::isNot(int type) {
  return this->type != type;
}

bool Token::isAny(std::vector<int> types) {
  for(auto type: types) {
    if (this->type == type) {
      return true;
    }
  }
  return false;
}

std::string Token::getLexeme() const {
  return lexeme;
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
  return os << t.lexeme << " r:"  << t.row << " c:" << t.col;
}
