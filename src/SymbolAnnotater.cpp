#include "SymbolAnnotator.h"
#include <cassert>

void SymbolAnnotator::visit(IntLiteral* t){}
void SymbolAnnotator::visit(DoubleLiteral* t){}
void SymbolAnnotator::visit(StringLiteral* t){}
void SymbolAnnotator::visit(Identifier* t){}
void SymbolAnnotator::visit(Type* t){}
void SymbolAnnotator::visit(Operator* t){}
void SymbolAnnotator::visit(BinaryExpr* t){
  t->left->context = t->context;
  t->left->accept(*this);
  t->right->context = t->context;
  t->right->accept(*this);
  t->op->context = t->context;
}
void SymbolAnnotator::visit(StmtList* t){
  t->stmt->context = t->context;
  t->stmt->accept(*this);
  if (t->next != nullptr) {
    t->next->context = t->context;
    t->next->accept(*this);
  }

}
void SymbolAnnotator::visit(BlockStmt* t){
  shared_ptr<SymbolTable> scope = make_shared<SymbolTable>(t->context);
  t->stmts->context = scope;
  t->stmts->accept(*this);
}
void SymbolAnnotator::visit(VarDecl* t){
  t->name->context = t->context;
  t->name->accept(*this);
  t->type->context = t->context;
  t->type->accept(*this);
}
void SymbolAnnotator::visit(FuncDecl* t){
  t->context->addSymbol(t->name->token.lexeme, "Function");
  t->name->context = t->context;
  t->name->accept(*this);
  if (t->params != nullptr) {
    t->params->context = t->context;
    t->params->accept(*this);
  }
  t->retType->context = t->context;
  t->retType->accept(*this);
  t->stmt->context = t->context;
  t->stmt->accept(*this);
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
