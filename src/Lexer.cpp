#include "Lexer.h"
#include "Token.h"

#include <fstream>
#include <set>
using std::string;

Lexer::Lexer(string path, ErrorReporter &err) : cLoc{-1}, cCol{-1}, error{err} {
  using std::ifstream;
  ifstream disk{path};

  while (!disk.eof()) {
    file.push_back(disk.get());
  }
}

bool Lexer::advanceIf(bool i) {
  if (i) {
    advance();
  } return i;
}

void Lexer::advance() {
  cLoc++;
  if (file[cLoc] == '\n') {
    cRow++;
    cCol = -1;
  } else {
    cCol++;
  }
}

char Lexer::at(int index) {
  return file[cLoc+index];
}

Token Lexer::lexIdentifier()  {
  string lexeme = string(1, at());
  while (std::isalpha(at(1))) {
    advance();
    lexeme += at();
  }


  if (lexeme == "var") {
    return Token(lexeme, Token::key_var, sLoc, sRow, sCol);
  } else if (lexeme == "let") {
    return Token(lexeme, Token::key_let, sLoc, sRow, sCol);
  } else if (lexeme == "func") {
    return Token(lexeme, Token::key_func, sLoc, sRow, sCol);
  } else if (lexeme == "if") {
    return Token(lexeme, Token::key_if, sLoc, sRow, sCol);
  } else if (lexeme == "while") {
    return Token(lexeme, Token::key_while, sLoc, sRow, sCol);
  } else if (lexeme == "return") {
    return Token(lexeme, Token::key_return, sLoc, sRow, sCol);
  } else {
    return Token(lexeme, Token::identifier, sLoc, sRow, sCol);
  }
}

Token Lexer::lexNumber() {
  string lexeme = string(1, at());

  while (std::isdigit(at(1))) {
    advance();
    lexeme += at();
  }

  if (at(1) == '.') {
    advance();
    lexeme += '.';

    while (std::isdigit(at(1))) {
      advance();
      lexeme += at();
    }
  }

  if (at(1) == 'e' || at(1) == 'E') {
    advance();
    lexeme += at();

    while (std::isdigit(at(1))) {
      advance();
      lexeme += at();
    }
  }

  return Token(lexeme, Token::number, sLoc, sRow, sCol);
}

auto  Lexer::lexCharLiteral() -> Token {
  string lexeme = string(1, at());
  advance();

  if (at() == '\\') {
    lexeme += '\\';
    advance();
  }

  lexeme += at();
  lexeme += at();

  return Token(lexeme, Token::char_literal, sLoc, sRow, sCol);
}

auto  Lexer::lexStringLiteral() -> Token {
  string lexeme = string(1, at());

  while (at(1) != '"' || at() == '\\') {
    lexeme += at();
    advance();
  }

  return Token(lexeme, Token::string_literal, sLoc, sRow, sCol);
}

void  Lexer::skipSlashStarComment() {
  while (at(1) != '*' || at(2) != '/') {
    advance();
  }
  advance();
  advance();
}

void  Lexer::skipSlashSlashComment() {
    while (at() != '\n') {
      advance();
    }
}

auto  Lexer::lexOperatorIdentifier() -> Token {
  std::string lexeme;

  switch(at()) {
    case '=':
      if (advanceIf(at(1) == '=')) lexeme = "==";
      else lexeme = "=";
      break;
    case '-':
      if (advanceIf(at(1) == '=')) lexeme = "-=";
      else if (advanceIf(at(1) == '-')) lexeme = "--";
      else if (advanceIf(at(1) == '>')) lexeme = "->";
      else lexeme = "-";
      break;
    case '+':
      if (advanceIf(at(1) == '=')) lexeme = "+=";
      else if (advanceIf(at(1) == '+')) lexeme = "++";
      else lexeme = "+";
      break;
    case '*':
      if (advanceIf(at(1) == '=')) lexeme = "*=";
      else lexeme = "*";
      break;
    case '/':
      if (advanceIf(at(1) == '=')) lexeme = "/=";
      else lexeme = "/";
      break;
    case '%':
      if (advanceIf(at(1) == '=')) lexeme = "%=";
      else lexeme = "%";
      break;
    case '<':
      if (advanceIf(at(1) == '=')) lexeme = "<=";
      else if (advanceIf(at(1) == '<')) {
        if (advanceIf(at(1) == '=')) lexeme = "<<=";
        else lexeme = "<<";
      } else lexeme = "<";
      break;
    case '>':
      if (advanceIf(at(1) == '=')) lexeme = ">=";
      else if (advanceIf(at(1) == '>')) {
        if (advanceIf(at(1) == '=')) lexeme = ">>=";
        else lexeme = ">>";
      } else lexeme = ">";
      break;
    case '&':
      if (advanceIf(at(1) == '=')) lexeme = "&=";
      else lexeme = "&";
      break;
    case '|':
      if (advanceIf(at(1) == '=')) lexeme = "|=";
      else if (advanceIf(at(1) == '|')) lexeme = "||";
      else lexeme = "|";
      break;
    case '^':
      if (advanceIf(at(1) == '=')) lexeme = "^=";
      else lexeme = "^";
      break;
    case '~':
      lexeme = "~";
      break;
    case '.':
      lexeme = ".";
      break;
    case '?':
      lexeme = "?";
      break;
    case '!':
      if (advanceIf(at(1) == '=')) lexeme = "!=";
      else lexeme = "!";
      break;
  }
  return Token(lexeme, Token::operator_identifier, sLoc, sRow, sCol);
}


Token Lexer::next() {
  Restart:
    advance();

    sLoc = cLoc;
    sRow = cRow;
    sCol = cCol;

    switch (at()) {

    default:
      error.report(sRow, sCol, "error: unknown token");
      return Token(string(1,at()), Token::unknown, sLoc, sRow, sCol);

    case '\n':
    case '\r':
      goto Restart;  // Skip whitespace.

    case ' ':
    case '\t':
    case '\f':
    case '\v':
      goto Restart;  // Skip whitespace.

    case -1:
      return Token("<eof>", Token::eof, sLoc, sRow, sCol);

    // Punctuation Characters
    case '{': return Token("{", Token::l_brace, sLoc, sRow, sCol);
    case '[': return Token("[", Token::l_square, sLoc, sRow, sCol);
    case '(': return Token("(", Token::l_paren, sLoc, sRow, sCol);
    case '}': return Token("}", Token::r_brace, sLoc, sRow, sCol);
    case ']': return Token("]", Token::r_square, sLoc, sRow, sCol);
    case ')': return Token(")", Token::r_paren, sLoc, sRow, sCol);
    case ',': return Token(",", Token::comma, sLoc, sRow, sCol);
    case ';': return Token(";", Token::semi, sLoc, sRow, sCol);
    case ':': return Token(":", Token::colon, sLoc, sRow, sCol);
    case '\\': return Token("\\", Token::backslash, sLoc, sRow, sCol);

    case '/':
      if (at(1) == '/') {
        skipSlashSlashComment();
        goto Restart;
      }
      if (at(1) == '*') {
        skipSlashStarComment();
        goto Restart;
      }
      return lexOperatorIdentifier();

    case '=': case '-': case '+': case '*': case '<': case '>': case '%':
    case '&': case '|':  case '^': case '~': case '.': case '?': case '!':
      return lexOperatorIdentifier();

    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':
    case '_':
      return lexIdentifier();

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      return lexNumber();

    case '"':
      return lexStringLiteral();

    case '\'':
      return lexCharLiteral();
    }
}
