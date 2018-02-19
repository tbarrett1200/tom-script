#include "Visitor.h"
#include <iostream>
#include "ErrorReporter.h"


void Visitor::visit(IntLiteral* i) {}
void Visitor::visit(DoubleLiteral* t) {}
void Visitor::visit(StringLiteral* t) {}
void Visitor::visit(Identifier* t) {}
void Visitor::visit(Type* t) {}
void Visitor::visit(OperatorNode* t) {}
void Visitor::visit(BinaryExpr* t) {
  t->left->accept(*this);
  if (t->op) {
    t->op->accept(*this);
    t->right->accept(*this);
  }
}
void Visitor::visit(UnaryExpr* t) {
  t->op->accept(*this);
  t->expr->accept(*this);
}

void Visitor::visit(StmtList* t) {
  t->stmt->accept(*this);
  if (t->next != nullptr) {
    t->next->accept(*this);
  }
}
void Visitor::visit(BlockStmt* t) {
  if (t->stmts) {
    t->stmts->accept(*this);
  }
}
void Visitor::visit(VarDecl* t) {
  t->name->accept(*this);
  if (t->type) {
    t->type->accept(*this);
  }
  if (t->value) {
    t->value->accept(*this);
  }
}
void Visitor::visit(ExprList* t) {
  t->stmt->accept(*this);
  if (t->next != nullptr) {
    t->next->accept(*this);
  }
}
void Visitor::visit(FunctionCall* t) {
  t->name->accept(*this);
  if (t->arguments) {
    t->arguments->accept(*this);
  }
}
void Visitor::visit(FuncDecl* t) {
  t->retType->accept(*this);
  t->name->accept(*this);
  if (t->params) {
    t->params->accept(*this);
  }
  t->stmt->accept(*this);
}
void Visitor::visit(IfStmt* t) {
  t->cond->accept(*this);
  t->stmt->accept(*this);
}
void Visitor::visit(WhileStmt* t) {
  t->cond->accept(*this);
  t->stmt->accept(*this);
}
void Visitor::visit(ExprStmt* t) {
  t->expr->accept(*this);
}
void Visitor::visit(ReturnStmt* t) {
  t->expr->accept(*this);
}
void Visitor::visit(Program* t) {
  t->block->accept(*this);
}
