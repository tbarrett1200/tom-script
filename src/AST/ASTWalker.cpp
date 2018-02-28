#include "AST/ASTWalker.h"

#include <stdexcept>
/*
bool ASTWalker::traverseNode(ASTNode* node) {
  switch (node->getType()) {
  case ASTNode::STMT:
    return traverseStmt(node->asStmt());
  case ASTNode::EXPR:
    return traverseExpr(node->asExpr());
  case ASTNode::DECL:
    return traverseDecl(node->asDecl());
  default: throw std::domain_error("ASTWalker: unrecognized node type");
  }
}
*/
bool ASTWalker::traverseDecl(Decl* d) {
  return walkUpFromDecl(d);
}
bool ASTWalker::traverseExpr(Expr* e) {
  return walkUpFromExpr(e);
}
bool ASTWalker::traverseStmt(Stmt* s) {
  return walkUpFromStmt(s);
}

/* responsible for walking higher */
bool ASTWalker::walkUpFromDecl(Decl* d) {
  return visitDecl(d);
}
bool ASTWalker::walkUpFromExpr(Expr* e) {
  return visitExpr(e);
}
bool ASTWalker::walkUpFromStmt(Stmt* s) {
  return visitStmt(s);
}

/* User Overridable Preorder Traversal */
bool ASTWalker::visitDecl(Decl*) {
  return true;
}
bool ASTWalker::visitExpr(Expr*) {
  return true;
}
bool ASTWalker::visitStmt(Stmt*) {
  return true;
}
