#include "Parse/Parser.h"
#include "AST/Type.h"

#include "Basic/CompilerException.h"

#include <memory>

using namespace std;


Type* Parser::parseType() {
  switch(token_.type()) {
    case Token::l_paren: return parseTupleOrFunctionType();
    case Token::identifier: return parseTypeIdentifier();
    case Token::l_square: return parseListOrMapType();
    default: throw CompilerException(token_.location(),  "error: unable to parse type");
  }
}

Type* Parser::parseTypeIdentifier() {
  auto token = expectToken(Token::identifier, "type identifier");
  if (token.lexeme() == StringRef{"Int"}) return IntegerType::getInstance();
  else if (token.lexeme()== StringRef{"Boolean"}) return BooleanType::getInstance();
  else if (token.lexeme()== StringRef{"Double"}) return DoubleType::getInstance();
  else {
    std::stringstream ss;
    ss << "error: unrecognized type identifier " << token.lexeme();
    throw CompilerException(token.location(), ss.str());
  }
}

std::vector<Type*> Parser::parseTupleTypeElementList() {
  std::vector<Type*> elements;
  elements.push_back(parseType());
  while (consumeToken(Token::comma)) {
    elements.push_back(parseType());
  }
  return elements;
}

TupleType* Parser::parseTupleType() {
  expectToken(Token::l_paren, "left parenthesis");
  std::vector<Type*> list;
  if (token_.isNot(Token::r_paren)) {
    list = parseTupleTypeElementList();
  }
  expectToken(Token::r_paren, "right parenthesis");
  return TupleType::getInstance(std::move(list));
}

FunctionType* Parser::parseFunctionType() {
  expectToken(Token::l_paren, "left parenthesis");
  std::vector<Type*> list;
  if (token_.isNot(Token::r_paren)){
    list = parseTupleTypeElementList();
  }
  expectToken(Token::r_paren, "right parenthesis");
  if (!consumeOperator("->")) throw CompilerException(token_.location(),  "error: expected ->");
  auto type = parseType();
  return FunctionType::getInstance(std::move(list), type);
}

Type* Parser::parseTupleOrFunctionType() {
  expectToken(Token::l_paren, "left parenthesis");
  std::vector<Type*> list;
  if (token_.isNot(Token::r_paren)) {
    list = parseTupleTypeElementList();
  }
  expectToken(Token::r_paren, "right parenthesis");
  if (!consumeOperator("->")) return TupleType::getInstance(std::move(list));
  auto type = parseType();
  return FunctionType::getInstance(std::move(list), type);
}

ListType* Parser::parseListType() {
  expectToken(Token::l_square, "left square bracket");
  auto type = parseType();
  expectToken(Token::r_square, "right square bracket");
  return ListType::getInstance(type);
}

MapType* Parser::parseMapType() {
  expectToken(Token::l_square, "left square bracket");
  auto keyType = parseType();
  expectToken(Token::colon, "colon");
  auto valueType = parseType();
  expectToken(Token::r_square, "right square bracket");
  return MapType::getInstance(keyType, valueType);
}

Type* Parser::parseListOrMapType() {
  expectToken(Token::l_square, "left square bracket");
  auto keyType = parseType();
  if (consumeToken(Token::colon)) {
    auto valueType = parseType();
    expectToken(Token::r_square, "right square bracket");
    return MapType::getInstance(keyType, valueType);
  }
  expectToken(Token::r_square, "right square bracket");
  return ListType::getInstance(keyType);
}
