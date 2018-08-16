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

#include <typeinfo>

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
  if (Decl *decl = this->currentContext->getDecl(expr.lexeme())) {
    expr.setType(decl->getType());
  } else {
    throw CompilerException(nullptr, "identifier not declared");
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
        throw CompilerException(nullptr, "parameter type does not match operator");

      expr.setType(func_type.getReturnType());

    } else {
      std::stringstream ss;
      ss <<  "not enough parameters to operator '" << expr.getOperator() << "'. expected 1 but got " << func_type.getParamCount();
      throw CompilerException(nullptr, ss.str());
    }
  } else throw CompilerException(nullptr, "operator not declared");
}
void TypeChecker::checkBinaryExpr(BinaryExpr &expr) {
  // first checks the types of the left and right hand side
  checkExpr(expr.getLeft());
  checkExpr(expr.getRight());
  // asserts that the operator is defined
  if (Decl *decl = this->currentContext->getDecl(expr.getOperator())) {
    if (FunctionType *func_type = dynamic_cast<FunctionType*>(decl->getType())) {
      // assert the the operator has two arguments
      if (func_type->getParamCount() == 2) {
        const Type* decl_left_param_type = func_type->getParam(0)->getCanonicalType();
        const Type* expr_left_param_type = expr.getLeft().getType()->getCanonicalType();
        // assert that the left argument matches the declaration
        if (decl_left_param_type != expr_left_param_type)
          throw CompilerException(nullptr, "left parameter type does not match operator");
        const Type* decl_right_param_type = func_type->getParam(1)->getCanonicalType();
        // assert that the right argument matches the declaration
        const Type* expr_right_param_type = expr.getLeft().getType()->getCanonicalType();
        if (decl_right_param_type != expr_right_param_type)
          throw CompilerException(nullptr, "right parameter type does not match operator");

        expr.setType(func_type->getReturnType());
      } else {
        std::stringstream ss;
        ss <<  "not enough parameters to operator '" << expr.getOperator() << "'. expected 2 but got " << func_type->getParamCount();
        throw CompilerException(nullptr, ss.str());
      }
    } else {
      std::stringstream ss;
      ss <<  "'" << expr.getOperator() << "' is not an operator";
      throw CompilerException(nullptr, ss.str());
    }
  } else {
    std::stringstream ss;
    ss <<  "'" << expr.getOperator() << "' not declared";
    throw CompilerException(nullptr, ss.str());
  }
}
void TypeChecker::checkFunctionCall(FunctionCall &expr) {
  for(auto &arg: expr.getArguments()) {
    checkExpr(*arg);
  }

  if (Decl *decl = this->currentContext->getDecl(expr.getFunctionName())) {
    if (FunctionType *func_type = dynamic_cast<FunctionType*>(decl->getType())) {
      if (func_type->getParamTypes().size() == expr.getArguments().size()) {
        for (int i = 0; i < func_type->getParamTypes().size(); i++) {
          const Type* arg = expr.getArguments()[i]->getType()->getCanonicalType();
          const Type* param = func_type->getParamTypes()[i]->getCanonicalType();
          if (arg != param) {
            std::stringstream ss;
            ss << "improper argument types passed to function '" << expr.getFunctionName() << "'";
            throw CompilerException(nullptr, ss.str());
          }
          expr.setType(func_type->getReturnType());
        }
      } else {
        std::stringstream ss;
        ss << "wrong number of arguments passed to function '" << expr.getFunctionName() << "'";
        throw CompilerException(nullptr, ss.str());
      }
    } else {
      std::stringstream ss;
      ss << "identifier '" << expr.getFunctionName() << "' is not a function";
      throw CompilerException(nullptr, ss.str());
    }
  } else {
    std::stringstream ss;
    ss << "function '" << expr.getFunctionName() << "' not declared";
    throw CompilerException(nullptr, ss.str());
  }
}
void TypeChecker::checkStringExpr(StringExpr &expr) {
  throw CompilerException(nullptr, "string expression not implemented");
}
void TypeChecker::checkListExpr(ListExpr &expr) {
  throw CompilerException(nullptr, "list expression not implemented");
}
void TypeChecker::checkTupleExpr(TupleExpr &expr) {
  throw CompilerException(nullptr, "tuple expression not implemented");
}
void TypeChecker::checkAccessorExpr(AccessorExpr &expr) {
  throw CompilerException(nullptr, "accessor expression not implemented");
}
