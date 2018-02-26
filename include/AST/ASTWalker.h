#ifndef AST_AST_WALKER_H
#define AST_AST_WALKER_H

#include "AST/ASTNode.h"

class ASTWalker {

bool traverseNode(ASTNode* node);

/* specifies child nodes */
bool traverseDecl(Decl*);
bool traverseExpr(Expr*);
bool traverseStmt(Stmt*);

/* specified parent node */
bool walkUpFromDecl(Decl*);
bool walkUpFromExpr(Expr*);
bool walkUpFromStmt(Stmt*);

/* user-defined action */
bool visitDecl(Decl*);
bool visitExpr(Expr*);
bool visitStmt(Stmt*);

};

#endif
