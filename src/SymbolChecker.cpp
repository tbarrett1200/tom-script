#include "SymbolChecker.h"
#include <cassert>


/*
SymbolChecker::SymbolChecker(SourceCode *src) : source{src} {};


void SymbolChecker::visit(IntLiteral* t){}
void SymbolChecker::visit(DoubleLiteral* t){}
void SymbolChecker::visit(StringLiteral* t){}
void SymbolChecker::visit(Identifier* t){
  if (!t->context->hasSymbol(t->token.lexeme)) {
    ErrorReporter{source}.report(t->token, std::string("Error: Undefined Identifier"));
  } else {
    std::string type = t->context->getType(t->token.lexeme);
    if (type == "Int") {
      t->type = Expr::Int;
    } else if (type == "Double") {
      t->type = Expr::Double;
    } else if (type == "String") {
      t->type = Expr::String;
    }
  }
}
void SymbolChecker::visit(Type* t){
  if (t->token.lexeme == "Int") {
    t->context->addSymbol(t->token.lexeme, "Int");
  } else if (t->token.lexeme == "Double") {
    t->context->addSymbol(t->token.lexeme, "Double");
  } else if (t->token.lexeme == "String") {
    t->context->addSymbol(t->token.lexeme, "String");
  } else {
    ErrorReporter{source}.report(t->token, "Error: Unrecognized Type");
  }
}
void SymbolChecker::visit(OperatorNode* t){}
void SymbolChecker::visit(BinaryExpr* t){
  t->left->accept(*this);
  t->right->accept(*this);
  t->op->accept(*this);

  if (t->left->type != t->right->type) {
    ErrorReporter{source}.report(t->op->token, "Error: Cannot convert type");
  }

}
void SymbolChecker::visit(StmtList* t){
  t->stmt->accept(*this);
  if (t->next != nullptr) {
    t->next->accept(*this);
  }
}
void SymbolChecker::visit(BlockStmt* t){
  t->stmts->accept(*this);
}
void SymbolChecker::visit(VarDecl* t){
  if (t->context->hasSymbol(t->name->token.lexeme, false)) {
    ErrorReporter{source}.report(t->name->token, "Error: Redefined Identifier");
  } else {
    t->context->addSymbol(t->name->token.lexeme, t->type->token.lexeme);
  }
  t->type->accept(*this);
}
void SymbolChecker::visit(FuncDecl* t){
  t->name->accept(*this);
  if (t->params != nullptr) {
    t->params->accept(*this);
  }
  t->retType->accept(*this);
  t->stmt->accept(*this);
}
void SymbolChecker::visit(FunctionCall* t){
  assert(false && "Error: Not Yet Implemented");
}
void SymbolChecker::visit(ExprList* t){
  assert(false && "Error: Not Yet Implemented");
}
void SymbolChecker::visit(IfStmt* t){
  t->cond->accept(*this);
  t->stmt->accept(*this);
}
void SymbolChecker::visit(WhileStmt* t){
  t->cond->accept(*this);
  t->stmt->accept(*this);
}
void SymbolChecker::visit(ExprStmt* t){
  t->expr->accept(*this);
}
void SymbolChecker::visit(ReturnStmt* t){
  t->expr->accept(*this);
}
void SymbolChecker::visit(Program* t){
  t->block->accept(*this);
}
*/
