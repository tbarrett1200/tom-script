#ifndef SEMA_TYPE_ANNOTATOR_H
#define SEMA_TYPE_ANNOTATOR_H

#include "AST/ASTWalker.h"
#include "AST/DeclarationContext.h"

#include <memory>

class TypeChecker : public ASTWalker {
public:
  DeclarationContext* context = DeclarationContext::globalContext.get();
  std::shared_ptr<Type> returns;

  bool visitStringExpr(std::shared_ptr<StringExpr> e);
  bool visitIntegerExpr(std::shared_ptr<IntegerExpr> e);
  bool visitDoubleExpr(std::shared_ptr<DoubleExpr> e);
  bool visitBoolExpr(std::shared_ptr<BoolExpr> e);
  bool visitLabeledExpr(std::shared_ptr<LabeledExpr> e);
  bool visitTupleExpr(std::shared_ptr<TupleExpr> e);
  bool visitIdentifierExpr(std::shared_ptr<IdentifierExpr> e);
  bool visitOperatorExpr(std::shared_ptr<OperatorExpr> e);
  bool visitUnaryExpr(std::shared_ptr<UnaryExpr> e);
  bool visitBinaryExpr(std::shared_ptr<BinaryExpr> e);
  bool visitFunctionCall(std::shared_ptr<FunctionCall> e);
  bool visitCompoundStmt(std::shared_ptr<CompoundStmt> s);
  bool visitTypeAlias(std::shared_ptr<TypeAlias> d);
  bool visitFuncDecl(std::shared_ptr<FuncDecl> d);
  bool visitReturnStmt(std::shared_ptr<ReturnStmt> s);
  bool visitParamDecl(std::shared_ptr<ParamDecl> d);
  bool visitLetDecl(std::shared_ptr<LetDecl> d);
  bool visitVarDecl(std::shared_ptr<VarDecl> d);
};

#endif
