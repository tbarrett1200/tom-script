#ifndef AST_STMT_H
#define AST_STMT_H

#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/List.h"

class Stmt {
public:
};

using StmtList = List<Stmt>;
using CompoundStmt = Meta<Stmt, StmtList>;
using ConditionalStmt = Meta<CompoundStmt, StmtList, Expr>;
using ConditionalStmtList = Meta<ConditionalStmt>;
using ConditionalStmtGroup = Meta<Stmt, ConditionalStmtList>;

#endif
