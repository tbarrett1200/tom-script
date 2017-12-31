#ifndef TREE_H
#define TREE_H

#include <vector>
#include <iostream>

#include "Token.h"

class ParseTree {
  int type;
  Token data;
  std::vector<ParseTree> children;
public:
  enum {
    Terminal, IntegerLiteral, Identifier, Operator,
    Expr, BinaryExpr,
    Decl, VarDecl, ConstDecl, FuncDecl, ParamDecl, ParamDeclList,
    Stmt, ExprStmt, IfStmt, WhileStmt, ReturnStmt, BlockStmt, DeclStmt, StmtList,
  };
  ParseTree(int, std::vector<ParseTree>);
  ParseTree(int, Token);
  ParseTree();
  friend std::ostream& operator<<(std::ostream& os, const ParseTree& dt);
};


#endif
