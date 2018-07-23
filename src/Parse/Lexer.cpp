#include "Parse/Lexer.h"
#include "Basic/ErrorReporter.h"

using std::string;

Lexer::Lexer(std::shared_ptr<SourceCode> src) : source{src}, cRow{0}, cCol{-1}, cLoc{-1} {}

bool Lexer::advanceIf(bool i) {
  if (i) {
    advance();
  } return i;
}

void Lexer::advance() {
  cLoc++;
  if (source->getChar(cLoc) == '\n') {
    cRow++;
    cCol = 0;
  } else {
    cCol++;
  }
}

char Lexer::at(int index) {
  return source->getChar(cLoc+index);
}

Token Lexer::lexIdentifier()  {
  string lexeme = string(1, at());
  while (std::isalnum(at(1)) || at(1)=='_') {
    advance();
    lexeme += at();
  }


  if (lexeme == "var") {
    return Token(lexeme, Token::kw_var, sLoc, sRow, sCol);
  } else if (lexeme == "let") {
    return Token(lexeme, Token::kw_let, sLoc, sRow, sCol);
  } else if (lexeme == "func") {
    return Token(lexeme, Token::kw_func, sLoc, sRow, sCol);
  } else if (lexeme == "if") {
    return Token(lexeme, Token::kw_if, sLoc, sRow, sCol);
  } else if (lexeme == "else") {
    return Token(lexeme, Token::kw_else, sLoc, sRow, sCol);
  } else if (lexeme == "while") {
    return Token(lexeme, Token::kw_while, sLoc, sRow, sCol);
  } else if (lexeme == "return") {
    return Token(lexeme, Token::kw_return, sLoc, sRow, sCol);
  } else if (lexeme == "true") {
    return Token(lexeme, Token::kw_true, sLoc, sRow, sCol);
  } else if (lexeme == "false") {
    return Token(lexeme, Token::kw_false, sLoc, sRow, sCol);
  } else if (lexeme == "typedef") {
    return Token(lexeme, Token::kw_typedef, sLoc, sRow, sCol);
  } else if (lexeme == "typealias") {
    return Token(lexeme, Token::kw_typealias, sLoc, sRow, sCol);
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
  } else {
    return Token(lexeme, Token::integer_literal, sLoc, sRow, sCol);
  }

  if (at(1) == 'e' || at(1) == 'E') {
    advance();
    lexeme += at();

    while (std::isdigit(at(1))) {
      advance();
      lexeme += at();
    }
  }

  if (std::isalpha(at(1))) {
    while(std::isalpha(at(1))) {
      advance();
      lexeme += at();
    }
    ErrorReporter{std::cerr, *source}.report(CompilerException({sRow, sCol}, "error: identifier cannot start with digit"));
    return Token(lexeme, Token::unknown, sLoc, sRow, sCol);
  }

  return Token(lexeme, Token::double_literal, sLoc, sRow, sCol);
}

auto  Lexer::lexStringLiteral() -> Token {
  string lexeme = string(1, at());

  while (at(1) != '"' || at() == '\\') {
    advance();
    lexeme += at();
  }

  advance();
  lexeme += at();

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
      else if (advanceIf(at(1) == '&')) lexeme = "&&";
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
  return Token(lexeme, Token::operator_id, sLoc, sRow, sCol);
}

Token Lexer::getEOF() {
  int loc = 0;
  int row = source->getLineCount() - 1;
  int col = source->getLine(row).length();
  return Token("<eof>", Token::eof, loc, row, col);
}

Token Lexer::next() {
  Restart:
    advance();

    sLoc = cLoc;
    sRow = cRow;
    sCol = cCol;

    switch (at()) {

    default:
      ErrorReporter{std::cerr, *source}.report(CompilerException({sRow, sCol}, "error: unknown token"));
      return Token(string(1,at()), Token::unknown, sLoc, sRow, sCol);

    case '\n':
    case '\r':
      cCol++;
      return Token("<new_line>", Token::new_line, sLoc, sRow, sCol);

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
    }
}
