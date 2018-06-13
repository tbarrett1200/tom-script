#include "Sema/TypeChecker.h"
#include "AST/DeclarationContext.h"
#include "AST/Type.h"
#include "AST/AmbiguousDecl.h"
#include "Parse/Parser.h"
#include "Basic/CompilerException.h"


bool TypeChecker::visitStringExpr(std::shared_ptr<StringExpr> e) {
  e->setType(Parser::makeType("String"));
  return false;
}

bool TypeChecker::visitIntegerExpr(std::shared_ptr<IntegerExpr> e) {
  e->setType(Parser::makeType("Int"));
  return false;
}

bool TypeChecker::visitDoubleExpr(std::shared_ptr<DoubleExpr> e) {
  e->setType(Parser::makeType("Double"));
  return false;
}

bool TypeChecker::visitBoolExpr(std::shared_ptr<BoolExpr> e) {
  e->setType(Parser::makeType("Bool"));
  return false;
}

bool TypeChecker::visitTupleExpr(std::shared_ptr<TupleExpr> e) {
  traverse(e->list);
  e->setType(std::make_shared<TupleType>(e->list->getTypeList()));
  return false;
}

bool TypeChecker::visitListExpr(std::shared_ptr<ListExpr> e) {
  traverse(e->data);
  auto firstElementType = (*e->data)[0]->getType();
  e->setType(std::make_shared<ListType>(firstElementType));
  for (int i = 0; i < e->data->size(); i++) {
    if (*firstElementType != (*(*e->data)[i]->getType())) {
      std::stringstream ss;
      ss << "error: list must have homogenous type " << (*e->data)[i]->getType() << " vs " << firstElementType ;
      throw CompilerException(e->getLocation(), ss.str());
    }
  }
  return false;
}

bool TypeChecker::visitLabeledExpr(std::shared_ptr<LabeledExpr> e) {
  traverse(e->expr);
  e->setType(std::make_shared<LabeledType>(std::make_shared<TypeLabel>(e->label->name), e->expr->getType()));
  return false;
}

bool TypeChecker::visitIdentifierExpr(std::shared_ptr<IdentifierExpr> e) {
  AmbiguousDecl decls = context->filter(e);

  if (decls.isEmpty()) {
    throw CompilerException(e->getLocation(), "error: " + e->getLexeme() + " has not been declared");
  } else if (decls.isAmbiguous()) {
    throw CompilerException(e->getLocation(), "error: " + e->getLexeme() + " is ambiguous");

  } else {
    e->setType(decls.get()->getType());
    e->decl = decls.get();
  }

  return false;
}

bool TypeChecker::visitOperatorExpr(std::shared_ptr<OperatorExpr> e) {
  AmbiguousDecl decls = context->filter([e](std::shared_ptr<Decl> d) {
    return d->getName() == e->getLexeme()
        && d->as<FuncDecl>()
        && *e->paramType == *d->getType()->as<FunctionType>()->params;
  });

  if (decls.isEmpty()) {
    std::stringstream ss;
    ss << "error: "  << e->getLexeme() << e->paramType << " has not been declared" ;
    throw CompilerException(e->getLocation(), ss.str());
  } else if (decls.isAmbiguous()) {
    std::stringstream ss;
    ss << "error: "  << e->getLexeme() << " is ambiguous" ;
    throw CompilerException(e->getLocation(), ss.str());
  } else {
    e->setType(decls.get()->getType());
    e->decl = std::dynamic_pointer_cast<FuncDecl>(decls.get());
  }

  return false;
}

bool TypeChecker::visitUnaryExpr(std::shared_ptr<UnaryExpr> e) {
  if (e->op->group.assignment && !e->expr->isLeftValue()) {
    std::stringstream ss;
    ss << "error: "  << "r-value reference in assignment" ;
    throw CompilerException(e->getLocation(), ss.str());
  }
  traverseExpr(e->expr);
  e->op->paramType = std::make_shared<TypeList>(std::vector<std::shared_ptr<Type>>{e->expr->getType()});
  traverseExpr(e->op);
  e->setType(e->op->getType()->as<FunctionType>()->returns);
  return false;
}

bool TypeChecker::visitBinaryExpr(std::shared_ptr<BinaryExpr> e) {
  traverseExpr(e->left);
  traverseExpr(e->right);

  if (e->op->group.assignment) {
    if (!e->left->isLeftValue()) {
      std::stringstream ss;
      ss << "error: "  << "r-value reference in assignment" ;
      throw CompilerException(e->getLocation(), ss.str());
    }
    if (*e->left->getType() != *e->right->getType()) {
      std::stringstream ss;
      ss << "error: "  << "assignment types do not match" ;
      throw CompilerException(e->getLocation(), ss.str());
    }
    e->setType(Parser::makeType("Void"));
    return false;
  }

  e->op->paramType = std::make_shared<TypeList>(std::vector<std::shared_ptr<Type>>{e->left->getType(), e->right->getType()});
  traverseExpr(e->op);
  e->setType(e->op->getType()->as<FunctionType>()->returns);
  return false;
}

bool TypeChecker::visitAccessorExpr(std::shared_ptr<AccessorExpr> e) {
  traverse(e->id);
  e->setType(e->id->getType());
  if (std::dynamic_pointer_cast<ListType>(e->getType())) {
    e->setType(std::dynamic_pointer_cast<ListType>(e->getType())->type);
  } else {
    throw CompilerException(e->getLocation(), "error: can only access list elements\n");
  }
  return false;
}

bool TypeChecker::visitFunctionCall(std::shared_ptr<FunctionCall> e) {
  traverse(e->arguments);

  AmbiguousDecl decls = context->filter([e](std::shared_ptr<Decl> d) {
    return d->getName() == e->name->getLexeme()
        && d->as<FuncDecl>()
        && ((e->arguments && d->getType()->as<FunctionType>()->params && *e->arguments->getTypeList() == *d->getType()->as<FunctionType>()->params)
        || (!e->arguments && !d->getType()->as<FunctionType>()->params));
  });

  if (decls.isEmpty()) {
    std::stringstream ss;
    ss << "error: "  << e->name->getLexeme() << " has not been declared" ;
    throw CompilerException(e->getLocation(), ss.str());
  } else if (decls.isAmbiguous()) {
    std::stringstream ss;
    ss << "error: "  << e->name->getLexeme() << " is ambiguous" ;
    throw CompilerException(e->getLocation(), ss.str());
  } else {
    e->setType(decls.get()->getType()->as<FunctionType>()->returns);
    e->decl = std::dynamic_pointer_cast<FuncDecl>(decls.get());
  }
  return false;
}

bool TypeChecker::visitCompoundStmt(std::shared_ptr<CompoundStmt> s) {
  s->context = std::make_shared<DeclarationContext>();
  s->context->setParent(context);
  context = s->context.get();
  traverse(s->list);
  context = context->getParent();
  return false;
}

bool TypeChecker::visitTypeAlias(std::shared_ptr<TypeAlias> d) {
  d->setContext(context);
  if (context->hasLocal(d)) {
    std::stringstream ss;
    ss << "error: redeclaration of " << *d ;
    throw CompilerException(d->getLocation(), ss.str());
  } else {
    context->add(d);
  }
  return false;
}

bool TypeChecker::visitFuncDecl(std::shared_ptr<FuncDecl> d) {
  // checks for redeclaration
  if (context->hasLocal(d)) {
    std::stringstream ss;
    ss << "error: redeclaration of " << *d ;
    throw CompilerException(d->getLocation(), ss.str());
  }
  // sets context for deeper checking
  context->add(d);
  d->setContext(context);
  context = d->getContext();

  if (d->params) {
    traverse(d->params);
  }

  // semantic checks
  if (d->stmt) {
    returns = d->type->returns;
    traverseCompoundStmt(d->stmt);
    returns = nullptr;
    if (*d->returnType != *Parser::makeType("Void") && !d->stmt->returns()) throw CompilerException(d->getLocation(), "error: doesn't return\n");
  }

  context = context->getParent();
  return false;
}

bool TypeChecker::visitReturnStmt(std::shared_ptr<ReturnStmt> s) {
  if (!returns) {
    std::stringstream ss;
    ss << "error: you can't return here dipshit " ;
    throw CompilerException(s->getLocation(), ss.str());
  } else if (returns && *returns != *Parser::makeType("Void") && !s->expr){
    std::stringstream ss;
    ss << "error: you gotta return something dipshit " ;
    throw CompilerException(s->getLocation(), ss.str());
  } else {
    traverseExpr(s->expr);
    if (*s->expr->getType() != *returns) {
      std::stringstream ss;
      ss << "error: you're returning the wrong type dipshit " << this->returns << " vs " << s->expr->getType() ;
      throw CompilerException(s->getLocation(), ss.str());
    }
  }
  return false;
}


bool TypeChecker::visitParamDecl(std::shared_ptr<ParamDecl> d) {
  auto var = std::make_shared<VarDecl>(d->secondary->token, d->type, nullptr);
  context->add(var, [](const DeclarationContext& c){
    return ComputedAddress::makeParam(c.getSize()+1);
  });
  return false;
}

bool TypeChecker::visitLetDecl(std::shared_ptr<LetDecl> d) {
  d->setContext(context);
  traverseExpr(d->expr);
  d->type = d->expr->getType();
  if (context->hasLocal(d)) {
    std::stringstream ss;
    ss << "error: redeclaration of " << *d ;
    throw CompilerException(d->getLocation(), ss.str());
  } else context->add(d);
  return false;
}

bool TypeChecker::visitVarDecl(std::shared_ptr<VarDecl> d) {
  d->setContext(context);
  traverseExpr(d->expr);
  d->type = d->expr->getType();
  if (context->hasLocal(d)) {
    std::stringstream ss;
    ss << "error: redeclaration of " << *d ;
    throw CompilerException(d->getLocation(), ss.str());
  } else context->add(d);
  return false;
}
