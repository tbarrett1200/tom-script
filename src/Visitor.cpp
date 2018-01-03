#include "Visitor.h"
#include <iostream>

void PrintVisitor::visit(IntegerLiteral* t) {
  std::cout << t->lexeme;
}
void PrintVisitor::visit(Identifier* t) {
  std::cout << t->lexeme;
}
void PrintVisitor::visit(Operator* t) {
  std::cout << t->lexeme;
}
void PrintVisitor::visit(BinaryExpr* t) {
  t->left->accept(*this);
  t->op->accept(*this);
  t->right->accept(*this);
}
void PrintVisitor::visit(StmtList* t) {
  t->stmt->accept(*this);
  if (t->next != nullptr)
    t->next->accept(*this);
}
void PrintVisitor::visit(BlockStmt* t) {
  std::cout << "{\n";
  t->stmts->accept(*this);
  std::cout << "}\n";
}
void PrintVisitor::visit(VarDecl* t) {
  t->type->accept(*this);
  std::cout << " ";
  t->name->accept(*this);
  std::cout << ";" << std::endl;
}
void PrintVisitor::visit(FuncDecl* t) {
  t->retType->accept(*this);
  std::cout << " ";
  t->name->accept(*this);
  std::cout << "(";
  if (t->params != nullptr)
    t->params->accept(*this);
  std::cout << ")";
  t->stmt->accept(*this);
}
void PrintVisitor::visit(IfStmt* t) {
  std::cout << "if ";
  std::cout << "(";
  t->cond->accept(*this);
  std::cout << ")";
  t->stmt->accept(*this);
}
void PrintVisitor::visit(WhileStmt* t) {
  std::cout << "while ";
  std::cout << "(";
  t->cond->accept(*this);
  std::cout << ")";
  t->stmt->accept(*this);
}
void PrintVisitor::visit(ExprStmt* t) {
  t->expr->accept(*this);
  std::cout << ";" << std::endl;
}
void PrintVisitor::visit(ReturnStmt* t) {
  std::cout << "return ";
  t->expr->accept(*this);
  std::cout << ";" << std::endl;
}
