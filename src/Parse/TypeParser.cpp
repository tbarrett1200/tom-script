#include "Parse/Parser.h"
#include "AST/TypeExpr.h"
#include "AST/List.h"

#include <memory>

using namespace std;

unique_ptr<Type> Parser::parseType() {
  switch(token().getType()) {
    case Token::l_paren: return parseTupleOrFunctionType();
    case Token::identifier: return parseTypeIdentifier();
    case Token::l_square: return parseListOrMapType();
    default:
      report(token(), "error: unable to parse type");
      return nullptr;
  }
}

unique_ptr<TypeIdentifier> Parser::parseTypeIdentifier() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return make_unique<TypeIdentifier>(tok);
  } else {
    report(token(), "error: expected type-identifier");
    return nullptr;
  }
}

unique_ptr<TypeLabel> Parser::parseTypeLabel() {
  Token tok = token();
  if (tok.is(Token::identifier)) {
    consume();
    return make_unique<TypeLabel>(tok);
  } else {
    report(token(), "error: expected type");
    return nullptr;
  }
}

unique_ptr<LabeledType> Parser::parseLabeledType() {
  auto label = parseTypeLabel();
  if (!label) {
    report(token(), "error: expected label");
    return nullptr;
    }
    consume();
    auto type = parseType();
    if (!type) {
      report(token(), "error: expected type");
      return nullptr;
    }
    return make_unique<LabeledType>(move(label), move(type));
}

unique_ptr<TypeList> Parser::parseTupleTypeElementList() {
  auto element = token(1).is(Token::colon) ? parseLabeledType() : parseType();
  if (!element) {
    report(token(), "error: expected type-list-element");
    return nullptr;
  }
  if (parseTerminal(Token::comma, ",", false)) {
    auto list = parseTupleTypeElementList();
    if (!element) {
      report(token(), "error: expected type-list");
      return nullptr;
    }
    return make_unique<TypeList>(move(element), move(list));
  } else return make_unique<TypeList>(move(element), nullptr);
}

unique_ptr<TupleType> Parser::parseTupleType() {
  if (!parseTerminal(Token::l_paren, "(", true)) return nullptr;
  auto list = token().is(Token::r_paren)? nullptr: parseTupleTypeElementList();
  if (!parseTerminal(Token::r_paren, ")", true)) return nullptr;
  return make_unique<TupleType>(move(list));
}

unique_ptr<FunctionType> Parser::parseFunctionType(bool decl = false) {
  if (!parseTerminal(Token::l_paren, "(", true)) return nullptr;
  auto list = token().is(Token::r_paren)? nullptr: parseTupleTypeElementList();
  if (!parseTerminal(Token::r_paren, ")", true)) return nullptr;

  if (!decl && list && list->has<LabeledType>()) {
    auto expr = dynamic_cast<LabeledType*>(list->element.get());
    auto label = expr->label->name;
    report(label, "warning: function type parameter elements may not be labeled");
  }

  if (!consumeOperator("->")) {
    report(token(), "error: expected ->");
    return nullptr;
  }

  auto type = parseType();
  if (!type) {
    report(token(), "error: expected return type");
    return nullptr;
  }
  return make_unique<FunctionType>(move(list), move(type));
}

unique_ptr<Type> Parser::parseTupleOrFunctionType() {
  if (!parseTerminal(Token::l_paren, "(", true)) return nullptr;
  auto list = token().is(Token::r_paren)? nullptr: parseTupleTypeElementList();
  if (!parseTerminal(Token::r_paren, ")", true)) return nullptr;
  if (!consumeOperator("->")) return make_unique<TupleType>(move(list));

  if (list && list->has<LabeledType>()) {
    auto expr = dynamic_cast<LabeledType*>(list->element.get());
    auto label = expr->label->name;
    report(label, "warning: function type parameter elements may not be labeled");
  }

  auto type = parseType();
  if (!type) {
    report(token(), "error: expected return type");
    return nullptr;
  }
  return make_unique<FunctionType>(move(list), move(type));
}

unique_ptr<ListType> Parser::parseListType() {
  if (!parseTerminal(Token::l_square, "[", true)) return nullptr;
  auto type = parseType();
  if (!type) {
    report(token(), "error: expected type");
    return nullptr;
  }
  if (!parseTerminal(Token::r_square, "]", true)) return nullptr;
  return make_unique<ListType>(move(type));
}

unique_ptr<MapType> Parser::parseMapType() {
  if (!parseTerminal(Token::l_square, "[", true)) return nullptr;
  auto keyType = parseType();
  if (!keyType) {
    report(token(), "error: expected type");
    return nullptr;
  }
  if (!parseTerminal(Token::colon, ":", true)) return nullptr;
  auto valueType = parseType();
  if (!valueType) {
    report(token(), "error: expected type");
    return nullptr;
  }
  if (!parseTerminal(Token::r_square, "]", true)) return nullptr;
  return make_unique<MapType>(move(keyType), move(valueType));
}

unique_ptr<Type> Parser::parseListOrMapType() {
  if (!parseTerminal(Token::l_square, "[", true)) return nullptr;
  auto keyType = parseType();
  if (!keyType) {
    report(token(), "error: expected type");
    return nullptr;
  }
  if (!parseTerminal(Token::colon, ":", false)) {
    if (!parseTerminal(Token::r_square, "]", false)) return nullptr;
    return make_unique<ListType>(move(keyType));
  }
  auto valueType = parseType();
  if (!valueType) {
    report(token(), "error: expected type");
    return nullptr;
  }
  if (!parseTerminal(Token::r_square, "]", true)) return nullptr;
  return make_unique<MapType>(move(keyType), move(valueType));
}
