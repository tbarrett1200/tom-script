#ifndef SEMA_INTERPRETER_H
#define SEMA_INTERPRETER_H

#include "AST/ASTWalker.h"
#include "AST/AmbiguousDecl.h"
#include "Sema/RuntimeStack.h"
#include "Parse/Parser.h"

#include <stack>
#include <memory>

class Interpreter : public ASTWalker {
  RuntimeStack stack;

  bool visitStringExpr(std::shared_ptr<StringExpr> e);
  bool visitIntegerExpr(std::shared_ptr<IntegerExpr> e);
  bool visitDoubleExpr(std::shared_ptr<DoubleExpr> e);
  bool visitBoolExpr(std::shared_ptr<BoolExpr> e);
  bool visitTupleExpr(std::shared_ptr<TupleExpr> e);
  bool visitListExpr(std::shared_ptr<ListExpr> e);
  bool visitIdentifierExpr(std::shared_ptr<IdentifierExpr> e);
  bool visitUnaryExpr(std::shared_ptr<UnaryExpr> s);
  bool visitBinaryExpr(std::shared_ptr<BinaryExpr> s);
  bool visitAccessorExpr(std::shared_ptr<AccessorExpr> s);
  bool visitFunctionCall(std::shared_ptr<FunctionCall> s);
  bool visitReturnStmt(std::shared_ptr<ReturnStmt> s);
  bool visitExprStmt(std::shared_ptr<ExprStmt> s);
  bool visitConditionalStmtList(std::shared_ptr<ConditionalStmtList> s);
  bool visitWhileLoop(std::shared_ptr<WhileLoop> s);
  bool visitVarDecl(std::shared_ptr<VarDecl> s);
  bool visitLetDecl(std::shared_ptr<LetDecl> s);
  bool visitFuncDecl(std::shared_ptr<FuncDecl> s);

};
#endif
