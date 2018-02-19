#include "SymbolAnnotator.h"
#include <cassert>

/*
void SymbolAnnotator::visit(BinaryExpr* t){
  t->left->context = t->context;
  t->right->context = t->context;
  Visitor::visit(t);
}

void SymbolAnnotator::visit(StmtList* t){
  t->stmt->context = t->context;
  if (t->next) {
    t->next->context = t->context;
  }
  Visitor::visit(t);
}

void SymbolAnnotator::visit(BlockStmt* t){
  shared_ptr<SymbolTable> scope = make_shared<SymbolTable>(t->context);
  t->stmts->context = scope;
  Visitor::visit(t);
}
void SymbolAnnotator::visit(VarDecl* t){
  t->name->context = t->context;
  t->type->context = t->context;
  Visitor::visit(t);

}
void SymbolAnnotator::visit(FuncDecl* t){
  t->context->addSymbol(t->name->token.lexeme, "Function");
  t->name->context = t->context;
  if (t->params) {
    t->params->context = t->context;
  }
  t->retType->context = t->context;
  t->stmt->context = t->context;
  Visitor::visit(t);
}
void SymbolAnnotator::visit(IfStmt* t){
  t->cond->context = t->context;
  t->cond->accept(*this);
  t->stmt->context = t->context;
  t->stmt->accept(*this);
}
void SymbolAnnotator::visit(WhileStmt* t){
  t->cond->context = t->context;
  t->cond->accept(*this);
  t->stmt->context = t->context;
  t->stmt->accept(*this);
}
void SymbolAnnotator::visit(ExprStmt* t){
  t->expr->context = t->context;
  t->expr->accept(*this);
}
void SymbolAnnotator::visit(FunctionCall* t){
  assert(false && "Error: Not Yet Implemented");
}
void SymbolAnnotator::visit(ExprList* t){
  assert(false && "Error: Not Yet Implemented");
}
void SymbolAnnotator::visit(ReturnStmt* t){
  t->expr->context = t->context;
  t->expr->accept(*this);
}
void SymbolAnnotator::visit(Program* t){
  t->block->accept(*this);
}
*/
