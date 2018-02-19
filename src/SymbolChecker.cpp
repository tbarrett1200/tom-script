#include "SymbolChecker.h"
#include "ErrorReporter.h"

#include <cassert>

SymbolChecker::SymbolChecker(SourceCode *src) : source{src} {};

void SymbolChecker::visit(Identifier* t) {
  if (!t->parent->symbols->hasSymbol(t->token.lexeme, true)) {
    ErrorReporter{source}.report(t->token, "error: symbol not found in scope");
  }
}

void SymbolChecker::visit(VarDecl* t) {
  if (t->parent->symbols->hasSymbol(t->name->token.lexeme, true)) {
    ErrorReporter{source}.report(t->name->token, "error: redefinition of symbol in scope");
  }
  std::string name = t->name->token.lexeme;
  std::string type = t->type ? t->type->token.lexeme : "";
  t->parent->symbols->registerSymbol(name, type);
}

void SymbolChecker::visit(FuncDecl* t){
  if (t->parent->symbols->hasSymbol(t->name->token.lexeme, true)) {
    ErrorReporter{source}.report(t->name->token, "error: redefinition of symbol in scope");
  }
  t->parent->symbols->registerSymbol(t->name->token.lexeme, t->retType->token.lexeme);
}
