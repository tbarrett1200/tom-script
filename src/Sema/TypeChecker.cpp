#include "Sema/TypeChecker.h"
#include "AST/DeclarationContext.h"
#include "AST/Type.h"
#include "AST/AmbiguousDecl.h"
#include "Parse/Parser.h"


bool TypeChecker::visitStringExpr(std::shared_ptr<StringExpr> e) {
  e->type = Parser::makeType("String");
  return false;
}

bool TypeChecker::visitIntegerExpr(std::shared_ptr<IntegerExpr> e) {
  e->type = Parser::makeType("Int");
  return false;
}

bool TypeChecker::visitDoubleExpr(std::shared_ptr<DoubleExpr> e) {
  e->type = Parser::makeType("Double");
  return false;
}

bool TypeChecker::visitBoolExpr(std::shared_ptr<BoolExpr> e) {
  e->type = Parser::makeType("Bool");
  return false;
}

bool TypeChecker::visitTupleExpr(std::shared_ptr<TupleExpr> e) {
  traverse(e->list);
  e->type = std::make_shared<TupleType>(e->list->getTypeList());
  return false;
}

bool TypeChecker::visitLabeledExpr(std::shared_ptr<LabeledExpr> e) {
  traverse(e->expr);
  e->type = std::make_shared<LabeledType>(std::make_shared<TypeLabel>(e->label->name), e->expr->type);
  return false;
}

bool TypeChecker::visitIdentifierExpr(std::shared_ptr<IdentifierExpr> e) {
  AmbiguousDecl decls = context->filter(e);

  if (decls.isEmpty()) {
    std::stringstream ss;
    ss << "error: "  << e->getLexeme() << " has not been declared" << std::endl;
    throw ss.str();
  } else if (decls.isAmbiguous()) {
    std::stringstream ss;
    ss << "error: "  << e->getLexeme() << " is ambiguous" << std::endl;
    throw ss.str();
  } else {
    e->type = decls.get()->getType();
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
    ss << "error: "  << e->getLexeme() << e->paramType << " has not been declared" << std::endl;
    throw ss.str();
  } else if (decls.isAmbiguous()) {
    std::stringstream ss;
    ss << "error: "  << e->getLexeme() << " is ambiguous" << std::endl;
    throw ss.str();
  } else {
    e->type = decls.get()->getType();
    e->decl = std::dynamic_pointer_cast<FuncDecl>(decls.get());
  }

  return false;
}

bool TypeChecker::visitUnaryExpr(std::shared_ptr<UnaryExpr> e) {
  if (e->op->group.assignment && !e->expr->isLeftValue()) {
    std::stringstream ss;
    ss << "error: "  << "r-value reference in assignment" << std::endl;
    throw ss.str();
  }
  traverseExpr(e->expr);
  e->op->paramType = std::make_shared<TypeList>(std::vector<std::shared_ptr<Type>>{e->expr->type});
  traverseExpr(e->op);
  e->type = e->op->type->as<FunctionType>()->returns;
  return false;
}

bool TypeChecker::visitBinaryExpr(std::shared_ptr<BinaryExpr> e) {
  if (e->op->group.assignment && !e->left->isLeftValue()) {
    std::stringstream ss;
    ss << "error: "  << "r-value reference in assignment" << std::endl;
    throw ss.str();
  }
  traverseExpr(e->left);
  traverseExpr(e->right);
  e->op->paramType = std::make_shared<TypeList>(std::vector<std::shared_ptr<Type>>{e->left->type, e->right->type});
  traverseExpr(e->op);
  e->type = e->op->type->as<FunctionType>()->returns;
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
    ss << "error: "  << e->name->getLexeme() << " has not been declared" << std::endl;
    throw ss.str();
  } else if (decls.isAmbiguous()) {
    std::stringstream ss;
    ss << "error: "  << e->name->getLexeme() << " is ambiguous" << std::endl;
    throw ss.str();
  } else {
    e->type = decls.get()->getType()->as<FunctionType>()->returns;
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
    ss << "error: redeclaration of " << *d << std::endl;
    throw ss;
  } else {
    context->add(d);
  }
  return false;
}

bool TypeChecker::visitFuncDecl(std::shared_ptr<FuncDecl> d) {
  // checks for redeclaration
  if (context->hasLocal(d)) {
    std::stringstream ss;
    ss << "error: redeclaration of " << *d << std::endl;
    throw ss.str();
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
    if (*d->returnType != *Parser::makeType("Void") && !d->stmt->returns()) throw std::string("error: doesn't return\n");
  }

  context = context->getParent();
  return false;
}

bool TypeChecker::visitReturnStmt(std::shared_ptr<ReturnStmt> s) {
  if (!returns) {
    std::stringstream ss;
    ss << "error: you can't return here dipshit " << std::endl;
    throw ss.str();
  } else if (returns && *returns != *Parser::makeType("Void") && !s->expr){
    std::stringstream ss;
    ss << "error: you gotta return something dipshit " << std::endl;
    throw ss.str();
  } else {
    traverseExpr(s->expr);
    if (*s->expr->type != *returns) {
      std::stringstream ss;
      ss << "error: you're returning the wrong type dipshit " << this->returns << " vs " << s->expr->type << std::endl;
      throw ss.str();
    }
  }
  return false;
}


bool TypeChecker::visitParamDecl(std::shared_ptr<ParamDecl> d) {
  auto var = std::make_shared<VarDecl>(d->secondary->token, d->type, nullptr);
  context->add(var, [](const DeclarationContext& c){
    return ComputedAddress::param(c.getSize());
  });
  return false;
}

bool TypeChecker::visitLetDecl(std::shared_ptr<LetDecl> d) {
  d->setContext(context);
  traverseExpr(d->expr);
  d->type = d->expr->type;
  if (context->hasLocal(d)) {
    std::stringstream ss;
    ss << "error: redeclaration of " << *d << std::endl;
    throw ss.str();
  } else context->add(d);
  return false;
}

bool TypeChecker::visitVarDecl(std::shared_ptr<VarDecl> d) {
  d->setContext(context);
  traverseExpr(d->expr);
  d->type = d->expr->type;
  if (context->hasLocal(d)) {
    std::stringstream ss;
    ss << "error: redeclaration of " << *d << std::endl;
    throw ss.str();
  } else context->add(d);
  return false;
}
