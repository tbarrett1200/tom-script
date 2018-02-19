#include "TypeChecker.h"

TypeChecker::TypeChecker(SourceCode* src) : source{src} {};

void TypeChecker::visit(IntLiteral* t) {
  t->qualifiedType = "Int";
}
void TypeChecker::visit(DoubleLiteral* t) {
  t->qualifiedType = "Double";
}
void TypeChecker::visit(StringLiteral* t) {
  t->qualifiedType = "String";
}
void TypeChecker::visit(Identifier* t) {
  t->qualifiedType = t->symbols->getType(t->token.lexeme);
}
void TypeChecker::visit(UnaryExpr* t) {
  Visitor::visit(t);
  t->qualifiedType = t->expr->qualifiedType;
}
void TypeChecker::visit(VarDecl* t) {
  Visitor::visit(t);
  if (t->type && t->value) {
    std::string typeA = t->value->qualifiedType;
    std::string typeB = t->type->token.lexeme;
    if (typeA != typeB) {
      ErrorReporter{source}.report(t->name->token, "error: declared type (" +
      typeB + ") and inferred type (" + typeA + ") do not match");
    }
  } else if (t->value) {
    t->symbols->registerSymbol(t->name->token.lexeme, t->value->qualifiedType);
  }

  if (t->type || t->value) {
    std::string qtype = t->symbols->getType(t->name->token.lexeme);

    if (!t->symbols->hasType(qtype, true)) {
      ErrorReporter{source}.report(t->name->token, "error: type is undefined in current context");
    }
  } else {
    ErrorReporter{source}.report(t->name->token, "error: expected type declaration");
  }

}
void TypeChecker::visit(BinaryExpr* t) {
  Visitor::visit(t);
  std::string typeA = t->left->qualifiedType;
  std::string typeB = t->right->qualifiedType;
  if (typeA == typeB) {
    t->qualifiedType = typeA;
  } else {
    ErrorReporter{source}.report(t->op->token, "error: unable to relate types " + typeA + " and " + typeB);
    t->qualifiedType = "Void";
  }
}
