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
      checkAccessorExpr(static_cast<AccessorExpr&>(expr));
      break;
    case Expr::Kind::BinaryExpr:
      checkBinaryExpr(static_cast<BinaryExpr&>(expr));
      break;
    case Expr::Kind::BoolExpr:
      checkBoolExpr(static_cast<BoolExpr&>(expr));
      break;
    case Expr::Kind::CharacterExpr:
      checkCharacterExpr(static_cast<CharacterExpr&>(expr));
      break;
    case Expr::Kind::DoubleExpr:
      checkDoubleExpr(static_cast<DoubleExpr&>(expr));
      break;
    case Expr::Kind::FunctionCall:
      checkFunctionCall(static_cast<FunctionCall&>(expr));
      break;
    case Expr::Kind::IdentifierExpr:
      checkIdentifierExpr(static_cast<IdentifierExpr&>(expr));
      break;
    case Expr::Kind::IntegerExpr:
      checkIntegerExpr(static_cast<IntegerExpr&>(expr));
      break;
    case Expr::Kind::ListExpr:
      checkListExpr(static_cast<ListExpr&>(expr));
      break;
    case Expr::Kind::StringExpr:
      checkStringExpr(static_cast<StringExpr&>(expr));
      break;
    case Expr::Kind::TupleExpr:
      checkTupleExpr(static_cast<TupleExpr&>(expr));
      break;
    case Expr::Kind::UnaryExpr:
      checkUnaryExpr(static_cast<UnaryExpr&>(expr));
      break;
  }

  if (!expr.getType()) {
    std::stringstream ss;
    ss <<  "unreachable: " << expr.name() << " did not properly set type";
    throw CompilerException(expr.location(), ss.str());
  }

}

void TypeChecker::checkPropertyAccessor(AccessorExpr &expr) {
  assert(expr.identifier().getType() != nullptr);
  if (expr.index().is<IdentifierExpr>()) {
    IdentifierExpr *id_expr = expr.index().as<IdentifierExpr>();
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
  checkExpr(expr.identifier());
  if (expr.identifier().isType<StructType>() || expr.identifier().isReferenceTo<StructType>()) {
    if (IdentifierExpr* id_expr = dynamic_cast<IdentifierExpr*>(&expr.index())) {
      checkPropertyAccessor(expr);
    } else if (AccessorExpr* accessor_expr = dynamic_cast<AccessorExpr*>(&expr.index())) {
      throw CompilerException(expr.location(), "cannot handle nested property accessors");
    } 
  } else {
    checkExpr(expr.index());
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

  FunctionSignature binary_op_signature {
    expr.getOperator(), {
      expr.getLeft().type()->getCanonicalType(),
      expr.getRight().type()->getCanonicalType()
    }
  };

  Decl *decl = currentContext->getDecl(binary_op_signature);
  FunctionType *func_type = static_cast<FunctionType*>(decl->canonical_type());
  expr.setType(func_type->getReturnType()->getCanonicalType());
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

  Decl *decl = currentContext->getDecl({expr.getFunctionName(), param_types});
  FunctionType *func_type = static_cast<FunctionType*>(decl->canonical_type());
  expr.setType(func_type->getReturnType()->getCanonicalType());
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
  expr.setType(
    ListType::getInstance(element_type, expr.elements().size())
  );
}

void TypeChecker::checkReferenceExpr(UnaryExpr &expr) {
  checkExpr(expr.getExpr());

  if (expr.getExpr().isLeftValue()) {
    auto referenced_type = expr.getExpr().getType()->getCanonicalType();
    auto expr_type = ReferenceType::getInstance(referenced_type);
    expr.setType(expr_type);
  } else {
    // unable to get the address of an expression which has no inherent
    // notion of location... e.g. a literal value
    std::stringstream ss;
    ss << "illegal attempt to reference a non-referenceable type: ";
    ss << expr.getExpr().getType()->toString() << std::endl;
    ss << "expression kind: " << expr.getExpr().name() << std::endl;
    throw CompilerException(expr.getOperator().start, ss.str());
  }
}

void TypeChecker::checkStringExpr(StringExpr &expr) {
  expr.setType(
    ListType::getInstance(CharacterType::getInstance(), expr.getString().size() + 1)
  );
}

void TypeChecker::checkTupleExpr(TupleExpr &expr) {
  std::vector<Type*> canonical_element_types;

  for (auto &element: expr.elements()) {
    checkExpr(*element);
    canonical_element_types.push_back(element->type()->getCanonicalType());
  }

  expr.setType(
    TupleType::getInstance(std::move(canonical_element_types))
  );
}

void TypeChecker::checkUnaryExpr(UnaryExpr &expr) {

  // reference expressions have a special set of rules, and
  // thus should be handled seperately.
  if (expr.getOperator() == StringRef{"&"}) {
    return checkReferenceExpr(expr);
  }

  // dereference expressions have a special set of rules, and
  // thus should be handled seperately.
  if (expr.getOperator() == StringRef{"*"}) {
    return checkDereferenceExpr(expr);
  }

  // before attempting to locate the operator being called, the type of the
  // single argument must be found
  checkExpr(expr.getExpr());

  // a function signature is constructed to search the identifier table for
  // a matching function
  FunctionSignature unary_op_signature{
    expr.getOperator()
  , {expr.getExpr().getType()->getCanonicalType()}
  };

  // this call will throw an exception if a unique, unambiguous function
  // is not found that matches the given signature
  Decl *decl = currentContext->getDecl(unary_op_signature);

  // the declaration found will ALWAYS be of function type, because a
  // function signature was passed in. Thus, it is safe to cast the
  // declaration type to a function type
  FunctionType* func_type = static_cast<FunctionType*>(decl->canonical_type());

  expr.setType(func_type->getReturnType()->getCanonicalType());
}
