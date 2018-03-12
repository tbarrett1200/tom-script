#ifndef AST_AST_WALKER_H
#define AST_AST_WALKER_H

#include "AST/Decl.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"
#include "AST/Type.h"

class ASTWalker {

/* specifies child nodes */
bool traverseDecl(Decl*);
bool traverseExpr(Expr*);
bool traverseStmt(Stmt*);
bool traverseNode(Matchable*);

/* specified parent node */
bool walkUpFromDecl(Decl*);
bool walkUpFromExpr(Expr*);
bool walkUpFromStmt(Stmt*);
bool walkUpFromType(Type*);

/* user-defined action */
bool visitDecl(Decl*);
bool visitExpr(Expr*);
bool visitStmt(Stmt*);
bool visitType(Type*);
};

#endif
