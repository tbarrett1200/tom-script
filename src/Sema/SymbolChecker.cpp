/*#include "SymbolChecker.h"
#include "ErrorReporter.h"

#include <cassert>
#include <iostream>

SymbolChecker::SymbolChecker(SourceCode *src) : source{src} {};

void SymbolChecker::visit(Identifier* t) {
  std::string name = t->token.lexeme;
  if (!t->scope->hasValueSymbol(name, true)) {
    ErrorReporter{source}.report(t->token, "error: symbol '" + name + "' not found in scope");
  }
}

void SymbolChecker::visit(VarDecl* t) {
  std::string name = t->name->token.lexeme;
  std::string type = t->type ? t->type->token.lexeme : "";

  if (t->scope->hasSymbol(name, false)) {
    ErrorReporter{source}.report(t->name->token, "error: redefinition of symbol in scope");
  } else {
    auto namedType = std::shared_ptr<Type>(new NamedType(type));
    auto symbol = std::shared_ptr<Symbol>(new ValueSymbol(name, namedType));
    t->scope->define(symbol);
  }
}

void SymbolChecker::visit(FuncDecl* t){
  auto name = t->name->token.lexeme;
  auto type = "Function";

  if (t->scope->hasSymbol(t->name->token.lexeme, true)) {
    ErrorReporter{source}.report(t->name->token, "error: redefinition of symbol in scope");
  } else {
    auto funcType =  std::shared_ptr<Type>(new FunctionType());
    auto symbol = std::shared_ptr<Symbol>(new ValueSymbol(name, func));
    t->parent->scope->define(symbol);
  }
}
*/
