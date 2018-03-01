#ifndef AST_AST_NODE_H
#define AST_AST_NODE_H

#include <memory>

using namespace std;

class Expr;
class Stmt;
class Decl;

struct ASTNode {
private:
  int type;
  union {
    Expr* expr;
    Stmt* stmt;
    Decl* decl;
  };
public:
  enum { EXPR, STMT, DECL };
  ASTNode(Expr*);
  ASTNode(Stmt*);
  ASTNode(Decl*);
  ~ASTNode();
  bool is(int);
  int getType();
  Expr* asExpr();
  Stmt* asStmt();
  Decl* asDecl();
};

#endif
