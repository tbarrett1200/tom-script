#include "Parse/Parser.h"
#include "AST/Type.h"

#include <memory>

using namespace std;

unique_ptr<Type> Parser::makeType(std::string text) {
  const std::stringstream sstream{text};
  auto source = SourceCode{sstream, "factory"};
  auto parser = Parser{&source};
  try {
    auto type = parser.parseType();
    return type;
  } catch (std::string) {
    return nullptr;
  }
}

unique_ptr<Type> Parser::parseType() {
  switch(token().getType()) {
    case Token::l_paren: return parseTupleOrFunctionType();
    case Token::identifier: return parseTypeIdentifier();
    case Token::l_square: return parseListOrMapType();
    default: throw report(token(), "error: unable to parse type");
  }
}

unique_ptr<TypeIdentifier> Parser::parseTypeIdentifier() {
  auto token = expectToken(Token::identifier, "type identifier");
  return make_unique<TypeIdentifier>(token);
}

unique_ptr<TypeLabel> Parser::parseTypeLabel() {
  auto token = expectToken(Token::identifier, "label");
  return make_unique<TypeLabel>(token);
}

unique_ptr<LabeledType> Parser::parseLabeledType() {
  auto label = parseTypeLabel();
  expectToken(Token::colon, "colon");
  auto type = parseType();
  return make_unique<LabeledType>(move(label), move(type));
}

unique_ptr<TypeList> Parser::parseTupleTypeElementList() {
  auto element = token(1).is(Token::colon) ? parseLabeledType() : parseType();
  if (acceptToken(Token::comma)) {
    auto list = parseTupleTypeElementList();
    return make_unique<TypeList>(move(element), move(list));
  } else return make_unique<TypeList>(move(element), nullptr);
}

unique_ptr<TupleType> Parser::parseTupleType() {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = token().is(Token::r_paren)? nullptr: parseTupleTypeElementList();
  expectToken(Token::l_paren, "right parenthesis");
  return make_unique<TupleType>(move(list));
}

unique_ptr<FunctionType> Parser::parseFunctionType(bool decl = false) {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = token().is(Token::r_paren)? nullptr: parseTupleTypeElementList();
  expectToken(Token::l_paren, "right parenthesis");

  if (!decl && list && list->has<LabeledType>()) {
    auto expr = dynamic_cast<LabeledType*>(list->element.get());
    auto label = expr->label->token;
    throw report(label, "error: function type parameter elements may not be labeled");
  }

  if (!consumeOperator("->")) throw report(token(), "error: expected ->");
  auto type = parseType();
  return make_unique<FunctionType>(move(list), move(type));
}

unique_ptr<Type> Parser::parseTupleOrFunctionType() {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = token().is(Token::r_paren)? nullptr: parseTupleTypeElementList();
  expectToken(Token::l_paren, "right parenthesis");
  if (!consumeOperator("->")) return make_unique<TupleType>(move(list));

  if (list && list->has<LabeledType>()) {
    auto expr = dynamic_cast<LabeledType*>(list->element.get());
    auto label = expr->label->token;
    throw report(label, "warning: function type parameter elements may not be labeled");
  }

  auto type = parseType();
  return make_unique<FunctionType>(move(list), move(type));
}

unique_ptr<ListType> Parser::parseListType() {
  expectToken(Token::l_square, "left square bracket");
  auto type = parseType();
  expectToken(Token::r_square, "right square bracket");
  return make_unique<ListType>(move(type));
}

unique_ptr<MapType> Parser::parseMapType() {
  expectToken(Token::l_square, "left square bracket");
  auto keyType = parseType();
  expectToken(Token::colon, "colon");
  auto valueType = parseType();
  expectToken(Token::r_square, "right square bracket");
  return make_unique<MapType>(move(keyType), move(valueType));
}

unique_ptr<Type> Parser::parseListOrMapType() {
  expectToken(Token::l_square, "left square bracket");
  auto keyType = parseType();
  if (!acceptToken(Token::colon)) {
    expectToken(Token::r_square, "right square bracket");
    return make_unique<ListType>(move(keyType));
  }
  auto valueType = parseType();
  expectToken(Token::r_square, "right square bracket");
  return make_unique<MapType>(move(keyType), move(valueType));
}
