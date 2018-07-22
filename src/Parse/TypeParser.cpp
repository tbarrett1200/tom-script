#include "Parse/Parser.h"
#include "AST/Type.h"
#include "Parse/Scope.h"

#include "Basic/CompilerException.h"

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
    default: throw CompilerException(token().getLocation(),  "error: unable to parse type");
  }
}

shared_ptr<Type> Parser::parseTypeIdentifier() {
  auto token = expectToken(Token::identifier, "type identifier");
  if (token.lexeme == "Integer") return IntegerType::getInstance();
  else if (token.lexeme == "Boolean") return BooleanType::getInstance();
  else if (token.lexeme == "Double") return DoubleType::getInstance();
  else {
    auto type = scope.getType(token.lexeme);
    if (type != nullptr) return type;
    else  throw CompilerException(token.getLocation(),  "error: unrecognized type identifier");
  }
}

std::vector<std::shared_ptr<Type>> Parser::parseTupleTypeElementList() {
  std::vector<std::shared_ptr<Type>> elements;
  elements.push_back(parseType());
  while (consumeToken(Token::comma)) {
    elements.push_back(parseType());
  }
  return elements;
}

shared_ptr<TupleType> Parser::parseTupleType() {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = token().is(Token::r_paren)? std::vector<std::shared_ptr<Type>>(): parseTupleTypeElementList();
  expectToken(Token::r_paren, "right parenthesis");
  return make_shared<TupleType>(move(list));
}

shared_ptr<FunctionType> Parser::parseFunctionType() {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = token().is(Token::r_paren)? std::vector<std::shared_ptr<Type>>(): parseTupleTypeElementList();
  expectToken(Token::r_paren, "right parenthesis");

  if (!consumeOperator("->")) throw CompilerException(token().getLocation(),  "error: expected ->");
  auto type = parseType();
  return make_shared<FunctionType>(move(list), move(type));
}

shared_ptr<Type> Parser::parseTupleOrFunctionType() {
  expectToken(Token::l_paren, "left parenthesis");
  auto list = token().is(Token::r_paren)? std::vector<std::shared_ptr<Type>>(): parseTupleTypeElementList();
  expectToken(Token::r_paren, "right parenthesis");
  if (!consumeOperator("->")) return make_shared<TupleType>(move(list));
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
