/*#include "Sema/TypeChecker.h"
#include <cassert>


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
  auto value = t->scope->getValue(t->token.lexeme, true);
  if (!value) {
    ErrorReporter{source}.report(t->token, "unable to qualify type");
    return;
  }
  t->qualifiedType = value->type;
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
    std::string name = t->name->token.lexeme;
    std::string type = t->value->qualifiedType;
    ErrorReporter{source}.report(t->name->token, "info: inferred " + name + " as " + type);
    std::shared_ptr<Value> val = t->scope->getValue(t->name->token.lexeme, true);
    val->type = type;
  }

  if (t->type || t->value) {
    std::string qtype = t->scope->getValue(t->name->token.lexeme, true)->type;
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
*/
