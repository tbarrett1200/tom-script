#ifndef AST_AST_NODE_H
#define AST_AST_NODE_H

#include <memory>

using namespace std;

class Expr;
class Stmt;
class Decl;

template <typename Parent, typename ...Members>
class Meta : public Parent {
  const std::tuple<unique_ptr<Members...>> members;
public:
  Meta(Members&&... m)
  : members{m...} {
    for (int i = 0; i < std::tuple_size<Members...>::value; i++) {
      if (get<i>(members) == nullptr) throw domain_error("null members not allowed");
    }
  }
};

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
