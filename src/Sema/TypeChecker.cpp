#include "Sema/TypeChecker.h"
#include "Basic/CompilerException.h"

#include "AST/DeclContext.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/Expr.h"

#include "Basic/CompilerException.h"
#include "Sema/TypeChecker.h"
#include "AST/DeclContext.h"
#include "AST/Expr.h"
#include "AST/Decl.h"
#include "AST/Type.h"


void TypeChecker::checkExpr(Expr &expr) {
  switch(expr.getKind()) {
    case Expr::Kind::IntegerExpr:
      checkIntegerExpr(dynamic_cast<IntegerExpr&>(expr));
      break;
    case Expr::Kind::DoubleExpr:
      checkDoubleExpr(dynamic_cast<DoubleExpr&>(expr));
      break;
    case Expr::Kind::BoolExpr:
      checkBoolExpr(dynamic_cast<BoolExpr&>(expr));
      break;
    case Expr::Kind::IdentifierExpr:
      checkIdentifierExpr(dynamic_cast<IdentifierExpr&>(expr));
      break;
    case Expr::Kind::UnaryExpr:
      checkUnaryExpr(dynamic_cast<UnaryExpr&>(expr));
      break;
    case Expr::Kind::BinaryExpr:
      checkBinaryExpr(dynamic_cast<BinaryExpr&>(expr));
      break;
    case Expr::Kind::FunctionCall:
      checkFunctionCall(dynamic_cast<FunctionCall&>(expr));
      break;
    case Expr::Kind::StringExpr:
      checkStringExpr(dynamic_cast<StringExpr&>(expr));
      break;
    case Expr::Kind::ListExpr:
      checkListExpr(dynamic_cast<ListExpr&>(expr));
      break;
    case Expr::Kind::TupleExpr:
      checkTupleExpr(dynamic_cast<TupleExpr&>(expr));
      break;
    case Expr::Kind::AccessorExpr:
      checkAccessorExpr(dynamic_cast<AccessorExpr&>(expr));
      break;
  }
}
void TypeChecker::checkIntegerExpr(IntegerExpr &expr) {
  expr.setType(IntegerType::getInstance());
}
void TypeChecker::checkDoubleExpr(DoubleExpr &expr) {
  expr.setType(DoubleType::getInstance());
}
void TypeChecker::checkBoolExpr(BoolExpr &expr) {
  expr.setType(BooleanType::getInstance());
}
void TypeChecker::checkIdentifierExpr(IdentifierExpr &expr) {
  if (Decl *decl = this->currentContext->getDecl(expr.getLexeme())) {
    expr.setType(decl->getType());
  } else {
    throw CompilerException(expr.getSourceRange().start, "identifier not declared");
  }
}
void TypeChecker::checkUnaryExpr(UnaryExpr &expr) {
  checkExpr(expr.getExpr());
  if (Decl *decl = this->currentContext->getDecl(expr.getOperator())) {
    FunctionType& func_type = dynamic_cast<FunctionType&>(*decl->getType());
    if (func_type.getParamCount() == 1) {
      const Type* decl_param_type = func_type.getParam(0)->getCanonicalType();
      const Type* expr_param_type = expr.getExpr().getType()->getCanonicalType();
      if (decl_param_type != expr_param_type)
        throw CompilerException(expr.getSourceRange().start, "parameter type does not match operator");

      expr.setType(func_type.getReturnType());

    } else throw CompilerException(expr.getSourceRange().start, "not enough parameters to operator");
  } else throw CompilerException(expr.getSourceRange().start, "operator not declared");
}
void TypeChecker::checkBinaryExpr(BinaryExpr &expr) {
  // first checks the types of the left and right hand side
  checkExpr(expr.getLeft());
  checkExpr(expr.getRight());
  // asserts that the operator is defined
  if (Decl *decl = this->currentContext->getDecl(expr.getOperator())) {
    FunctionType& func_type = dynamic_cast<FunctionType&>(*decl->getType());
    // assert the the operator has two arguments
    if (func_type.getParamCount() == 2) {
      const Type* decl_left_param_type = func_type.getParam(0)->getCanonicalType();
      const Type* expr_left_param_type = expr.getLeft().getType()->getCanonicalType();
      // assert that the left argument matches the declaration
      if (decl_left_param_type != expr_left_param_type)
        throw CompilerException(expr.getSourceRange().start, "left parameter type does not match operator");
      const Type* decl_right_param_type = func_type.getParam(1)->getCanonicalType();
      // assert that the right argument matches the declaration
      const Type* expr_right_param_type = expr.getLeft().getType()->getCanonicalType();
      if (decl_right_param_type != expr_right_param_type)
        throw CompilerException(expr.getSourceRange().start, "right parameter type does not match operator");

      expr.setType(func_type.getReturnType());
    } else throw CompilerException(expr.getSourceRange().start, "not enough parameters to operator");
  } else throw CompilerException(expr.getSourceRange().start, "operator '" + expr.getOperator() + "' not declared");
}
void TypeChecker::checkFunctionCall(FunctionCall &expr) {
  for(auto arg: expr.getArguments()) {
    checkExpr(*arg);
  }

  if (Decl *decl = this->currentContext->getDecl(expr.getFunctionName())) {
    if (FunctionType *func_type = dynamic_cast<FunctionType*>(decl->getType().get())) {
      if (func_type->getParamTypes().size() == expr.getArguments().size()) {
        for (int i = 0; i < func_type->getParamTypes().size(); i++) {
          const Type* arg = expr.getArguments()[i]->getType()->getCanonicalType();
          const Type* param = func_type->getParamTypes()[i]->getCanonicalType();
          if (arg != param)  throw CompilerException(expr.getSourceRange().start, "parameter type mismatch");
        }
      } else throw CompilerException(expr.getSourceRange().start, "not the right number of arguments to function");
    } else throw CompilerException(expr.getSourceRange().start, "identifier is not a function");
  } else throw CompilerException(expr.getSourceRange().start, "identifier not declared");
}
void TypeChecker::checkStringExpr(StringExpr &expr) {
  throw CompilerException(expr.getSourceRange().start, "string expression not implemented");
}
void TypeChecker::checkListExpr(ListExpr &expr) {
  throw CompilerException(expr.getSourceRange().start, "list expression not implemented");
}
void TypeChecker::checkTupleExpr(TupleExpr &expr) {
  throw CompilerException(expr.getSourceRange().start, "tuple expression not implemented");
}
void TypeChecker::checkAccessorExpr(AccessorExpr &expr) {
  throw CompilerException(expr.getSourceRange().start, "accessor expression not implemented");
}
