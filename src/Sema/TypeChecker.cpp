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
    case Expr::Kind::AccessorExpr:
      checkAccessorExpr(dynamic_cast<AccessorExpr&>(expr));
      break;
    case Expr::Kind::BinaryExpr:
      checkBinaryExpr(dynamic_cast<BinaryExpr&>(expr));
      break;
    case Expr::Kind::BoolExpr:
      checkBoolExpr(dynamic_cast<BoolExpr&>(expr));
      break;
    case Expr::Kind::CharacterExpr:
      checkCharacterExpr(dynamic_cast<CharacterExpr&>(expr));
      break;
    case Expr::Kind::DoubleExpr:
      checkDoubleExpr(dynamic_cast<DoubleExpr&>(expr));
      break;
    case Expr::Kind::FunctionCall:
      checkFunctionCall(dynamic_cast<FunctionCall&>(expr));
      break;
    case Expr::Kind::IdentifierExpr:
      checkIdentifierExpr(dynamic_cast<IdentifierExpr&>(expr));
      break;
    case Expr::Kind::IntegerExpr:
      checkIntegerExpr(dynamic_cast<IntegerExpr&>(expr));
      break;
    case Expr::Kind::ListExpr:
      checkListExpr(dynamic_cast<ListExpr&>(expr));
      break;
    case Expr::Kind::StringExpr:
      checkStringExpr(dynamic_cast<StringExpr&>(expr));
      break;
    case Expr::Kind::TupleExpr:
      checkTupleExpr(dynamic_cast<TupleExpr&>(expr));
      break;
    case Expr::Kind::UnaryExpr:
      checkUnaryExpr(dynamic_cast<UnaryExpr&>(expr));
      break;
    default:
      throw CompilerException(expr.location(), "typecheck: unimplemented: " + expr.name());
  }

  if (!expr.getType()) {
    std::stringstream ss;
    ss <<  "typecheck: unexpected: " << expr.name() << " did not properly set type";
    throw CompilerException(expr.location(), ss.str());
  }

}

void TypeChecker::checkPropertyAccessor(AccessorExpr &expr) {
  checkExpr(expr.identifier());
  if (IdentifierExpr* id_expr = dynamic_cast<IdentifierExpr*>(&expr.index())) {
    Type* id_type = expr.identifier().getType()->getCanonicalType();
    if (StructType *struct_type = dynamic_cast<StructType*>(id_type)) {
      int member_index = struct_type->index_of(id_expr->lexeme().str());
      if (member_index == -1) {
        throw CompilerException(expr.location(), "property '" + id_expr->lexeme().str() + "' not found in struct ");
      } else {
        expr.setMemberIndex(member_index);
        expr.setType(struct_type->type_of_member_at(member_index)->getCanonicalType());
      }
    } else if (expr.identifier().isReferenceTo<StructType>()) {
      StructType *struct_type = dynamic_cast<StructType*>(dynamic_cast<ReferenceType*>(id_type)->getReferencedType()->getCanonicalType());
      int member_index = struct_type->index_of(id_expr->lexeme().str());
      if (member_index == -1) {
        throw CompilerException(expr.location(), "property '" + id_expr->lexeme().str() + "' not found in struct ");
      } else {
        expr.setMemberIndex(member_index);
        expr.setType(struct_type->type_of_member_at(member_index)->getCanonicalType());
      }
    } else throw CompilerException(expr.location(), "cannot access property of value of type " + expr.identifier().getType()->toString());
  } else throw CompilerException(expr.location(), "bad thing happened");

}

void TypeChecker::checkElementAccessor(AccessorExpr &expr) {
  checkExpr(expr.identifier());
  checkExpr(expr.index());
  // two possible cases: list or &list
  if (expr.identifier().isType<ListType>()) {
    expr.setType(expr.identifier().type()->as<ListType>()->element_type());
  } else if (expr.identifier().isReferenceTo<ListType>()) {
    ReferenceType* ref_type = expr.identifier().type()->as<ReferenceType>();
    expr.setType(ref_type->getReferencedType()->as<ListType>()->element_type());
  } else if (expr.identifier().isType<SliceType>()) {
    expr.setType(expr.identifier().type()->as<SliceType>()->element());
  } else {
    std::stringstream ss;
    ss << "illegal attempt to access element of " << expr.identifier().type()->toString() << ". ";
    ss << "element accessors may only be used on aggregate data types, such as an array";
    throw CompilerException(expr.location(), ss.str());
  }
}

// An accessor Expression comes in two main flavours: property access and
// index access. A property access only works on types with named properties,
// such as a struct. Each property corresponds to a struct element index.
// A index access directly specifies the index to access. This method checks
// which type of accessor is being passed, and delegates it to the appropriate
// method
void TypeChecker::checkAccessorExpr(AccessorExpr &expr) {
  if (IdentifierExpr* id_expr = dynamic_cast<IdentifierExpr*>(&expr.index())) {
    checkPropertyAccessor(expr);
  } else if (AccessorExpr* accessor_expr = dynamic_cast<AccessorExpr*>(&expr.index())) {
    throw CompilerException(expr.location(), "cannot handle nested property accessors");
  } else {
    checkElementAccessor(expr);
  }
}

bool TypeChecker::is_implicitly_assignable_to(Type *l, Type *r) {
  if (l->getCanonicalType() == r->getCanonicalType()) {
    return true;
  } else if (SliceType *slice_type = dynamic_cast<SliceType*>(l->getCanonicalType())) {
    if (ReferenceType *ref_type = dynamic_cast<ReferenceType*>(r->getCanonicalType())) {
      if (ref_type->getReferencedType() == slice_type->element()) {
        return true;
      }
    }
  }
  return false;
}

void TypeChecker::checkAssignmentExpr(BinaryExpr &expr) {
  if (expr.getLeft().isLeftValue()) {
    Type* ltype = expr.getLeft().getType()->getCanonicalType();
    Type* rtype = expr.getRight().getType()->getCanonicalType();
    expr.setType(expr.getRight().getType()->getCanonicalType());
    if (ltype != rtype) {
      std::stringstream ss;
      ss <<  "mismatched type for assignment operands " << ltype->toString();
      throw CompilerException(expr.location(), ss.str());
    }
  } else {
    std::stringstream ss;
    ss <<  "unable to assign to left hand side";
    throw CompilerException(expr.location(), ss.str());
  }
}

void TypeChecker::checkBinaryExpr(BinaryExpr &expr) {
  // must check the type of all sub-expressions first
  checkExpr(expr.getLeft());
  checkExpr(expr.getRight());

  // special members
  if (expr.getOperator() == StringRef{"="}) {
    return checkAssignmentExpr(expr);
  }

  // asserts that the operator is defined
  std::vector<Type*> param_types{
    expr.getLeft().type()->getCanonicalType(),
    expr.getRight().type()->getCanonicalType()
  };

  if (Decl *decl = currentContext->getDecl({expr.getOperator(), param_types})) {

    if (FunctionType *func_type = decl->getType()->as_canonical<FunctionType>()) {
      // a binary expression, by definiton, must have exactly two parameters.
      if (func_type->getParamCount() == 2) {
        expr.setType(func_type->getReturnType());
      } else {
        std::stringstream ss;
        ss <<  "wrong number parameters to operator '" << expr.getOperator();
        ss << "'. expected 2 but got " << func_type->getParamCount();
        throw CompilerException(expr.location(), ss.str());
      }
    } else {
      std::stringstream ss;
      ss <<  "'" << expr.getOperator() << "' is not an operator";
      throw CompilerException(expr.location(), ss.str());
    }
  } else {
    std::stringstream ss;
    ss <<  "the '" << expr.getOperator() << "' operator has not been declared for argument types ";
    ss << expr.getLeft().getType()->toString() << " and ";
    ss << expr.getRight().getType()->toString();
    throw CompilerException(expr.location(), ss.str());
  }
}

void TypeChecker::checkBoolExpr(BoolExpr &expr) {
  expr.setType(BooleanType::getInstance());
}

void TypeChecker::checkCharacterExpr(CharacterExpr &expr) {
  expr.setType(CharacterType::getInstance());
}

void TypeChecker::checkDereferenceExpr(UnaryExpr &expr) {
  checkExpr(expr.getExpr());
  if ( PointerType* ptr_type = expr.getExpr().getType()->getCanonicalType()->as<PointerType>()) {
    expr.setType(ptr_type->getReferencedType());
  } else if ( ReferenceType* ref_type = expr.getExpr().getType()->getCanonicalType()->as<ReferenceType>()) {
    expr.setType(ref_type->getReferencedType());
  } else {
    std::stringstream ss;
    ss << "illegal attempt to dereference a non-reference type.";
    throw CompilerException(expr.getOperator().start, ss.str());
  }
}

void TypeChecker::checkDoubleExpr(DoubleExpr &expr) {
  expr.setType(DoubleType::getInstance());
}


void TypeChecker::checkFunctionCall(FunctionCall &expr) {
  // must check the type of all sub-expressions first
  for(auto &arg: expr.getArguments()) {
    checkExpr(*arg);
  }

  std::vector<Type*> param_types;
  for(auto &arg: expr.getArguments()) {
    param_types.push_back(arg->getType()->getCanonicalType());
  }

  if (Decl *decl = currentContext->getDecl({expr.getFunctionName(), param_types})) {
    if ( FunctionType *func_type = decl->getType()->getCanonicalType()->as<FunctionType>()) {
      if (func_type->getParamTypes().size() == expr.getArguments().size()) {
        expr.setType(func_type->getReturnType()->getCanonicalType());
      } else {
        std::stringstream ss;
        ss << "wrong number of arguments passed to function '";
        ss << expr.getFunctionName() << "'. expected ";
        ss << func_type->getParamTypes().size() << " but found " << expr.getArguments().size();
        throw CompilerException(expr.location(), ss.str());
      }
    } else {
      std::stringstream ss;
      ss << "'" << expr.getFunctionName() << "' is not declared a function";
      throw CompilerException(expr.location(), ss.str());
    }
  } else {
    std::stringstream ss;
    ss << "function '" << expr.getFunctionName() << "' not declared with parameters (";
    for (auto param: param_types) {
      ss << param->toString() << ", ";
    }
    ss.seekp((int)ss.tellp()-2);
    ss << ").";
    throw CompilerException(expr.location(), ss.str());
  }
}

void TypeChecker::checkIdentifierExpr(IdentifierExpr &expr) {
  if (Decl *decl = currentContext->getDecl(expr.lexeme())) {
    if (Type *type = decl->getType()) {
      expr.setDecl(decl);
      expr.setType(type->getCanonicalType());
    } else throw CompilerException(expr.location(), "fatal: declaration missing type");
  } else {
    std::stringstream ss;
    ss << "identifier '" << expr.lexeme() << "' has not been declared";
    throw CompilerException(expr.location(), ss.str());
  }
}

void TypeChecker::checkIntegerExpr(IntegerExpr &expr) {
  expr.setType(IntegerType::getInstance());
}


void TypeChecker::checkListExpr(ListExpr &expr) {
  if (expr.elements().size() == 0) {
    throw CompilerException(expr.location(), "array literal must have at least one element");
  }
  Type *element_type = nullptr;
  for (auto &element: expr.elements()) {
    checkExpr(*element);
    if (element_type) {
      if (element->getType()->getCanonicalType() != element_type) {
        throw CompilerException(expr.location(), "array literal must be composed of elements of a single type");
      }
    } else element_type = element->getType()->getCanonicalType();
  }
  expr.setType(ListType::getInstance(element_type, expr.elements().size()));
}

void TypeChecker::checkReferenceExpr(UnaryExpr &expr) {
  checkExpr(expr.getExpr());
  // unable to get the address of an expression which has no inherent
  // notion of location... e.g. a literal value
  if (expr.getExpr().isLeftValue()) {
    //if (expr.getExpr().type()->getKind() == Type::Kind::ListType) {
    //   ListType* list_type = dynamic_cast< ListType*>(expr.getExpr().type());
    //  expr.setType(SliceType::getInstance(list_type->element_type()));
    //} else {
      expr.setType(ReferenceType::getInstance(expr.getExpr().getType()->getCanonicalType()));
  //  }
  } else {
    std::stringstream ss;
    ss << "illegal attempt to reference a non-referenceable type: " << expr.getExpr().getType()->toString() << std::endl;
    ss << "expression kind: " << expr.getExpr().name() << std::endl;
    throw CompilerException(expr.getOperator().start, ss.str());
  }
}

void TypeChecker::checkStringExpr(StringExpr &expr) {

  expr.setType(ListType::getInstance(CharacterType::getInstance(), expr.getString().size() + 1));
}

void TypeChecker::checkTupleExpr(TupleExpr &expr) {
  std::vector<Type*> element_types;
  for (auto &element: expr.elements()) {
    checkExpr(*element);
    element_types.push_back(element->type());
  }
  expr.setType(TupleType::getInstance(std::move(element_types)));
}

void TypeChecker::checkUnaryExpr(UnaryExpr &expr) {
  if (expr.getOperator() == StringRef{"&"}) {
    return checkReferenceExpr(expr);
  } else if (expr.getOperator() == StringRef{"*"}) {
    return checkDereferenceExpr(expr);
  } else {
    checkExpr(expr.getExpr());
    std::vector<Type*> param_types{ expr.getExpr().getType()->getCanonicalType() };
    if (Decl *decl = currentContext->getDecl(FunctionSignature(expr.getOperator(), param_types))) {
      if ( FunctionType* func_type = decl->getType()->getCanonicalType()->as<FunctionType>()) {
        expr.setType(func_type->getReturnType()->getCanonicalType());
      } else {
        std::stringstream ss;
        ss <<  "'" << expr.getOperator() << "' is not an operator";
        throw CompilerException(expr.location(), ss.str());
      }
    } else {
      std::stringstream ss;
      ss <<  "the unary '" << expr.getOperator() << "' operator has not been declared for argument type ";
      ss << expr.getExpr().getType()->toString() << " and ";
      throw CompilerException(expr.location(), ss.str());
    }
  }
}
