#include "AST/ASTNode.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"

ASTNode::ASTNode(Expr* e){
  type = ASTNode::EXPR;
  expr = e;
}

ASTNode::ASTNode(Stmt* s){
  type = ASTNode::STMT;
  stmt = s;
}

ASTNode::ASTNode(Decl* d){
  type = ASTNode::DECL;
  decl = d;
}

ASTNode::~ASTNode() {
  switch (type) {
  case EXPR:
    delete expr;
    break;
  case STMT:
    delete stmt;
    break;
  case DECL:
    delete decl;
    break;
  }
}

bool ASTNode::is(int t){
  return type == t;
}

int ASTNode::getType() {
  return type;
}

Expr* ASTNode::asExpr(){
  if (type == ASTNode::EXPR) {
    return expr;
  } else return nullptr;
}

Stmt* ASTNode::asStmt(){
  if (type == ASTNode::STMT) {
    return stmt;
  } else return nullptr;
}

Decl* ASTNode::asDecl(){
  if (type == ASTNode::DECL) {
    return decl;
  } else return nullptr;
}
