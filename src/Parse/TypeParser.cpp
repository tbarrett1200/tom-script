#include "Parse/Parser.h"
#include "AST/Type.h"

#include <memory>

using namespace std;

shared_ptr<Type> Parser::makeType(std::string text) {
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

shared_ptr<Type> Parser::parseType() {
  switch(token().getType()) {
    case Token::l_paren: return parseTupleOrFunctionType();
    case Token::identifier: return parseTypeIdentifier();
    case Token::l_square: return parseListOrMapType();
    default: throw report(token(), "error: unable to parse type");
  }
}

shared_ptr<TypeIdentifier> Parser::parseTypeIdentifier() {
  auto token = expectToken(Token::identifier, "type identifier");
  return make_shared<TypeIdentifier>(token);
}

shared_ptr<TypeLabel> Parser::parseTypeLabel() {
  auto token = expectToken(Token::identifier, "label");
  return make_shared<TypeLabel>(token);
}

shared_ptr<LabeledType> Parser::parseLabeledType() {
  auto label = parseTypeLabel();
  expectToken(Token::colon, "colon");
  auto type = parseType();
  return make_shared<LabeledType>(move(label), move(type));
}

shared_ptr<TypeList> Parser::parseTupleTypeElementList() {
  auto element = token(1).is(Token::colon) ? parseLabeledType() : parseType();
  if (consumeToken(Token::comma)) {
    auto list = parseTupleTypeElementList();
    return make_shared<TypeList>(move(element), move(list));
  } else return make_shared<TypeList>(move(element), nullptr);
}

shared_ptr<TupleType> Parser::parseTupleType() {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = token().is(Token::r_paren)? nullptr: parseTupleTypeElementList();
  expectToken(Token::r_paren, "right parenthesis");
  return make_shared<TupleType>(move(list));
}

shared_ptr<FunctionType> Parser::parseFunctionType(bool decl = false) {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = token().is(Token::r_paren)? nullptr: parseTupleTypeElementList();
  expectToken(Token::r_paren, "right parenthesis");

  if (!decl && list && list->has<LabeledType>()) {
    auto expr = dynamic_cast<LabeledType*>(list->element.get());
    auto label = expr->label->token;
    throw report(label, "error: function type parameter elements may not be labeled");
  }

  if (!consumeOperator("->")) throw report(token(), "error: expected ->");
  auto type = parseType();
  return make_shared<FunctionType>(move(list), move(type));
}

shared_ptr<Type> Parser::parseTupleOrFunctionType() {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = token().is(Token::r_paren)? nullptr: parseTupleTypeElementList();
  expectToken(Token::r_paren, "right parenthesis");
  if (!consumeOperator("->")) return make_shared<TupleType>(move(list));

  if (list && dynamic_cast<LabeledType*>(list->element.get())) {
    auto expr = dynamic_cast<LabeledType*>(list->element.get());
    auto label = expr->label->token;
    throw report(label, "warning: function type parameter elements may not be labeled");
  }

  auto type = parseType();
  return make_shared<FunctionType>(move(list), move(type));
}

shared_ptr<ListType> Parser::parseListType() {
  expectToken(Token::l_square, "left square bracket");
  auto type = parseType();
  expectToken(Token::r_square, "right square bracket");
  return make_shared<ListType>(move(type));
}

shared_ptr<MapType> Parser::parseMapType() {
  expectToken(Token::l_square, "left square bracket");
  auto keyType = parseType();
  expectToken(Token::colon, "colon");
  auto valueType = parseType();
  expectToken(Token::r_square, "right square bracket");
  return make_shared<MapType>(move(keyType), move(valueType));
}

shared_ptr<Type> Parser::parseListOrMapType() {
  expectToken(Token::l_square, "left square bracket");
  auto keyType = parseType();
  if (consumeToken(Token::colon)) {
    auto valueType = parseType();
    expectToken(Token::r_square, "right square bracket");
    return make_shared<MapType>(move(keyType), move(valueType));
  }
  expectToken(Token::r_square, "right square bracket");
  return make_shared<ListType>(move(keyType));
}
