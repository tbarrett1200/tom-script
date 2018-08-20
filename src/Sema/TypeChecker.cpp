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
    case Expr::Kind::CharacterExpr:
      checkCharacterExpr(dynamic_cast<CharacterExpr&>(expr));
      break;
    case Expr::Kind::AccessorExpr:
      checkAccessorExpr(dynamic_cast<AccessorExpr&>(expr));
      break;
  }
  if (!expr.getType()) throw std::logic_error(expr.name() + " did not properly set type");
}
void TypeChecker::checkIntegerExpr(IntegerExpr &expr) {
  expr.setType(IntegerType::getInstance());
}
void TypeChecker::checkDoubleExpr(DoubleExpr &expr) {
  expr.setType(DoubleType::getInstance());
}

void TypeChecker::checkCharacterExpr(CharacterExpr &expr) {
  expr.setType(CharacterType::getInstance());
}

void TypeChecker::checkBoolExpr(BoolExpr &expr) {
  expr.setType(BooleanType::getInstance());
}
void TypeChecker::checkIdentifierExpr(IdentifierExpr &expr) {
  if (Decl *decl = currentContext->getDecl(expr.lexeme())) {
    expr.setType(decl->getType());
  } else {
    std::stringstream ss;
    ss << "identifier '" << expr.lexeme() << "' not declared";
    throw CompilerException(nullptr, ss.str());
  }
}
void TypeChecker::checkUnaryExpr(UnaryExpr &expr) {
  checkExpr(expr.getExpr());

  if (expr.getOperator() == StringRef{"&"}) {
    if (expr.getExpr().isLeftValue()) {
      const Type* ltype = expr.getExpr().getType()->getCanonicalType();
      expr.setType(PointerType::getInstance(ltype));
    } else {
      std::stringstream ss;
      ss <<  "unable to reference r-value";
      throw CompilerException(expr.getOperator().start, ss.str());
    }
  } else if (expr.getOperator() == StringRef{"*"}) {
    if (expr.getExpr().getType()->getKind() == Type::Kind::PointerType) {
      const PointerType* type = dynamic_cast<const PointerType*>(expr.getExpr().getType());
      expr.setType(ReferenceType::getInstance(type->getReferencedType()));
    } else {
      std::stringstream ss;
      ss <<  "dereference operator only valid for pointer types";
      throw CompilerException(expr.getOperator().start, ss.str());
    }
  } else {
    std::vector<const Type*> param_types{ expr.getExpr().getType() };
    if (Decl *decl = this->currentContext->getDecl(FunctionSignature(expr.getOperator(), param_types))) {
      const FunctionType& func_type = dynamic_cast<const FunctionType&>(*decl->getType());
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
}
void TypeChecker::checkBinaryExpr(BinaryExpr &expr) {
  // first checks the types of the left and right hand side
  checkExpr(expr.getLeft());
  checkExpr(expr.getRight());

  if (expr.getOperator() == StringRef{"="}) {
    if (expr.getLeft().isLeftValue()) {
      const Type* ltype = expr.getLeft().getType()->getCanonicalType();
      const Type* rtype = expr.getRight().getType()->getCanonicalType();
      expr.setType(expr.getRight().getType());
      if (ltype != rtype) {
        std::stringstream ss;
        ss <<  "mismatched type for assignment operands";
        throw CompilerException(expr.getOperator().start, ss.str());
      }
    } else {
      std::stringstream ss;
      ss <<  "unable to assign to left hand side";
      throw CompilerException(expr.getOperator().start, ss.str());
    }
  } else {
      // asserts that the operator is defined
      std::vector<const Type*> param_types{ expr.getLeft().getType(), expr.getRight().getType() };
      if (Decl *decl = this->currentContext->getDecl({expr.getOperator(), param_types})) {
        if (const FunctionType *func_type = dynamic_cast<const FunctionType*>(decl->getType())) {
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
            ss <<  "not enough parameters to operator '" << expr.getOperator();
            ss << "'. expected 2 but got " << func_type->getParamCount();
            throw CompilerException(nullptr, ss.str());
          }
        } else {
          std::stringstream ss;
          ss <<  "'" << expr.getOperator() << "' is not an operator";
          throw CompilerException(nullptr, ss.str());
        }
      } else {
        std::stringstream ss;
        ss <<  "'" << expr.getOperator() << "' not declared for ";
        ss << expr.getLeft().getType()->toString() << " and ";
        ss << expr.getRight().getType()->toString();
        throw CompilerException(nullptr, ss.str());
      }
  }
}
void TypeChecker::checkFunctionCall(FunctionCall &expr) {
  for(auto &arg: expr.getArguments()) {
    checkExpr(*arg);
  }
  std::vector<const Type*> param_types;
  for(auto &arg: expr.getArguments()) {
    param_types.push_back(arg->getType());
  }
  if (Decl *decl = this->currentContext->getDecl({expr.getFunctionName(),param_types})) {
    if (const FunctionType *func_type = dynamic_cast<const FunctionType*>(decl->getType())) {
      if (func_type->getParamTypes().size() == expr.getArguments().size()) {
        for (int i = 0; i < func_type->getParamTypes().size(); i++) {
          const Type* arg = expr.getArguments()[i]->getType()->getCanonicalType();
          const Type* param = func_type->getParamTypes()[i]->getCanonicalType();
          if (arg != param) {
            std::stringstream ss;
            ss << "improper argument types passed to function '" << expr.getFunctionName() << "'";
            throw CompilerException(nullptr, ss.str());
          }
        }
        expr.setType(func_type->getReturnType());
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
    ss << "function '" << expr.getFunctionName() << "' not declared with parameters (";
    for (auto param: param_types) {
      ss << param->toString() << ", ";
    }
    ss.seekp((int)ss.tellp()-2);
    ss << ").";
    throw CompilerException(nullptr, ss.str());
  }
}
void TypeChecker::checkStringExpr(StringExpr &expr) {
  expr.setType(ListType::getInstance(CharacterType::getInstance(), expr.getString().size() + 1));
}

void TypeChecker::checkListExpr(ListExpr &expr) {
  if (expr.elements().size() == 0) {
    throw CompilerException(nullptr, "list must have at least one element");
  }

  const Type *element_type = nullptr;
  for (auto &element: expr.elements()) {
    checkExpr(*element);
    if (element_type) {
      if (element->getType()->getCanonicalType() != element_type) {
        throw CompilerException(nullptr, "list must be composed of elements of a single type");
      }
    } else element_type = element->getType()->getCanonicalType();
  }
  expr.setType(ListType::getInstance(element_type, expr.elements().size()));
}

void TypeChecker::checkTupleExpr(TupleExpr &expr) {
  throw CompilerException(nullptr, "tuple expression not implemented");
}
void TypeChecker::checkAccessorExpr(AccessorExpr &expr) {
  checkExpr(expr.identifier());
  const Type *aggregate_type = expr.identifier().getType();

  if (aggregate_type->getKind() == Type::Kind::ReferenceType) {
    const ReferenceType *ref_type = dynamic_cast<const ReferenceType*>(aggregate_type);
    aggregate_type = ref_type->getReferencedType();
  }

  switch(aggregate_type->getKind()) {
    case Type::Kind::ListType: {
      const ListType *list_type = dynamic_cast<const ListType*>(aggregate_type);
      expr.setType(list_type->element_type());
      if (list_type->size() <= expr.index()) {
        std::stringstream ss;
        ss << "illegal attempt to access index " << expr.index();
        ss << " of array of size " << list_type->size();
        throw CompilerException(nullptr, ss.str());
      }
      break;
    }
    default: {
      std::stringstream ss;
      ss << "illegal element access of type " << aggregate_type->toString() << ". ";
      ss << "element accessors may only be used on aggregate data types, such as an array";
      throw CompilerException(nullptr, ss.str());
    }
  }
}
