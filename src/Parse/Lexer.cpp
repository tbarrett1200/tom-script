#include "Parse/Lexer.h"
#include "Basic/ErrorReporter.h"

using std::string;

Lexer::Lexer(std::shared_ptr<SourceFile> src) : source{src}, source_iterator{src->begin()} {}

Token Lexer::lexIdentifier()  {
  const char *start = current_loc();

  while (source_iterator != source->end() && (std::isalnum(*source_iterator) || *source_iterator == '_')) {
    source_iterator++;
  }

  int length = current_loc() - start;
  StringRef str_ref{start, length};

  if (str_ref == StringRef{"var"}) {
    return Token(Token::kw_var, str_ref);
  } else if (str_ref == StringRef{"let"}) {
    return Token(Token::kw_let, str_ref);
  } else if (str_ref == StringRef{"func"}) {
    return Token(Token::kw_func, str_ref);
  } else if (str_ref == StringRef{"if"}) {
    return Token(Token::kw_if, str_ref);
  } else if (str_ref == StringRef{"else"}) {
    return Token(Token::kw_else, str_ref);
  } else if (str_ref == StringRef{"then"}) {
    return Token(Token::kw_then, str_ref);
  } else if (str_ref == StringRef{"while"}) {
    return Token(Token::kw_while, str_ref);
  } else if (str_ref == StringRef{"return"}) {
    return Token(Token::kw_return, str_ref);
  } else if (str_ref == StringRef{"true"}) {
    return Token(Token::kw_true, str_ref);
  } else if (str_ref == StringRef{"false"}) {
    return Token(Token::kw_false, str_ref);
  } else if (str_ref == StringRef{"extern"}) {
    return Token(Token::kw_extern, str_ref);
  } else if (str_ref == StringRef{"typedef"}) {
    return Token(Token::kw_typedef, str_ref);
  } else if (str_ref == StringRef{"typealias"}) {
    return Token(Token::kw_typealias, str_ref);
  } else {
    return Token(Token::identifier, str_ref);
  }
}

Token Lexer::lexNumber() {
  const char* start = current_loc();
  bool floating_point = false;

  // lex integer or pre-radix mantissa
  while (std::isdigit(*source_iterator)) {
    source_iterator++;
    if (source_iterator == source->end()) {
      return Token(Token::integer_literal, start, current_loc() - start);
    }
  }

  // lex post-radix mantissa
  if (*source_iterator == '.') {
    floating_point = true;
    source_iterator++;
    if (source_iterator == source->end()) {
      return Token(Token::double_literal, start, current_loc() - start);
    }
    while (std::isdigit(*source_iterator)) {
      source_iterator++;
      if (source_iterator == source->end()) {
        return Token(Token::double_literal, start, current_loc() - start);
      }
    }
  }

  // lex exponent
  if (*source_iterator == 'e' || *source_iterator == 'E') {
    floating_point = true;
    source_iterator++;
    while (source_iterator != source->end() && std::isdigit(*source_iterator)) {
      source_iterator++;
    }
  }

  return Token(floating_point ? Token::double_literal: Token::integer_literal, start, current_loc() - start);
}

auto  Lexer::lexStringLiteral() -> Token {
  const char *loc = current_loc();

  if (*source_iterator != '"') {
    throw std::logic_error("error: expected '\"' while lexing string literal");
  } else source_iterator++;

  while (source_iterator != source->end() || *source_iterator == '"') {
    source_iterator++;
  }
  if (source_iterator == source->end()) {
    throw std::logic_error("error: expected '\"' while lexing string literal");
  } else source_iterator++;

  return Token(Token::string_literal, loc, current_loc() - loc);
}


Token  Lexer::lexCharacterLiteral() {
  const char *loc = current_loc();

  if (*source_iterator != '\'') {
    throw std::logic_error("error: expected '\"' while lexing string literal");
  } else source_iterator++;
  if (*source_iterator == '\\') {
    source_iterator++;
    switch (*source_iterator) {
      case 'n':
      case '0':
        source_iterator++;
        break;
      default: throw CompilerException(&*source_iterator, "invalid character escape");
    }
  } else source_iterator++;
  if (*source_iterator != '\'') {
    throw std::logic_error("error: expected '\"' while lexing string literal");
  } else source_iterator++;

  return Token(Token::character_literal, loc, current_loc() - loc);
}

void  Lexer::lexSlashStarComment() {
  while (source_iterator != source->end()) {
    if (*source_iterator == '*') {
      source_iterator++;
      if (source_iterator != source->end()) {
        if (*source_iterator == '/') {
          source_iterator++;
          return;
        }
      } else return;
    } else source_iterator++;
  }
}

void  Lexer::lexSlashSlashComment() {
  while (source_iterator != source->end()) {
    if (*source_iterator == '\n') {
      source_iterator++;
      return;
    } else {
      source_iterator++;
    }
  }
}

Token Lexer::lexOperatorIdentifier() {
  const char* start = current_loc();

  switch(*source_iterator) {
    case '=':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      break;
    case '-':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      else if (*source_iterator == '-') source_iterator++;
      else if (*source_iterator == '>') source_iterator++;
      break;
    case '+':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      else if (*source_iterator == '+') source_iterator++;
      break;
    case '*':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      break;
    case '/':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      break;
    case '%':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      break;
    case '<':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      else if (*source_iterator == '<') {
        source_iterator++;
        if (*source_iterator == '=') source_iterator++;
      }
      break;
    case '>':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      else if (*source_iterator == '>') {
        source_iterator++;
        if (*source_iterator == '=') source_iterator++;
      }
      break;
    case '&':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      else if (*source_iterator == '&') source_iterator++;
      break;
    case '|':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      else if (*source_iterator == '|') source_iterator++;
      break;
    case '^':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      break;
    case '~':
      source_iterator++;
      break;
    case '?':
      source_iterator++;
      break;
    case '!':
      source_iterator++;
      if (*source_iterator == '=') source_iterator++;
      break;
  }
  return Token(Token::operator_id, start, current_loc() - start);
}


const char* Lexer::current_loc() const {
  return &(*source_iterator);
}

Token Lexer::lexPunctuation() {
  Token tok;
  switch(*source_iterator) {
  case '{':
    tok = Token(Token::l_brace, current_loc(), 1);
    break;
  case '[':
    tok = Token(Token::l_square, current_loc(), 1);
    break;
  case '(':
    tok = Token(Token::l_paren, current_loc(), 1);
    break;
  case '}':
    tok = Token(Token::r_brace, current_loc(), 1);
    break;
  case ']':
    tok = Token(Token::r_square, current_loc(), 1);
    break;
  case ')':
    tok = Token(Token::r_paren, current_loc(), 1);
    break;
  case ',':
    tok = Token(Token::comma, current_loc(), 1);
    break;
  case '.':
    tok = Token(Token::dot, current_loc(), 1);
    break;
  case ';':
    tok = Token(Token::semi, current_loc(), 1);
    break;
  case ':':
    tok = Token(Token::colon, current_loc(), 1);
    break;
  case '\\':
    tok = Token(Token::backslash, current_loc(), 1);
    break;
  case '\n':
    tok = Token(Token::new_line, current_loc(), 1);
    break;
  }

  source_iterator++;
  return tok;
}

Token Lexer::next() {
  for (auto &it = source_iterator; it != source->end(); it++) {
    switch(*it) {
    // skip whitespace
    case ' ':
    case '\t':
    case '\f':
    case '\v':
      continue;

    // newline

    // Punctuation Characters
    case '\n': case '{': case '[': case '(':
    case '}': case ']': case ')': case ',':
    case ';': case ':': case '\\': case '.':
      return lexPunctuation();

    case '/':
      source_iterator++;
      if (*source_iterator == '/') {
        lexSlashSlashComment();
        continue;
      }
      if (*source_iterator == '*') {
        lexSlashStarComment();
        continue;
      }
      source_iterator--;
      return lexOperatorIdentifier();

    case '=': case '-': case '+': case '*': case '<': case '>': case '%':
    case '&': case '|':  case '^': case '~':  case '?': case '!':
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

    case '\'':
      return lexCharacterLiteral();

    case '"':
      return lexStringLiteral();
    }
  }
  return Token(Token::eof, &(*source->begin()) + source->content_length(), 0);
}
