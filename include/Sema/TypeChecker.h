#ifndef SEMA_TYPE_ANNOTATOR_H
#define SEMA_TYPE_ANNOTATOR_H

#include "AST/ASTWalker.h"
#include "AST/AmbiguousDecl.h"
#include "Driver/GlobalContext.h"

class TypeChecker : public ASTWalker {
public:
  DeclarationContext* context = globalContext.get();
  std::shared_ptr<Type> returns;

  bool visitLabeledExpr(std::shared_ptr<LabeledExpr> e) {
    traverseExpr(e->expr);
    e->type = e->expr->type.label(e->label);
    return false;
  }

  bool visitIdentifierExpr(std::shared_ptr<IdentifierExpr> e) {
    auto decls = context->filter(e);

    if (decls.isEmpty()) {
      std::stringstream ss;
      ss << "error: "  << e->getLexeme() << " has not been declared" << std::endl;
      throw ss.str();
    } else if (decls.isAmbiguous()) {
      std::stringstream ss;
      ss << "error: "  << e->getLexeme() << " is ambiguous" << std::endl;
      throw ss.str();
    } else {
      e->type = decls.getTypes();
      e->decl = decls.get();
    }

    return false;
  }

  bool visitTupleExpr(std::shared_ptr<TupleExpr> e) {
    traverse(e->list);
    AmbiguousTypeList typeList {e->list};
    std::vector<std::shared_ptr<Type>> types;
    for (auto p: typeList.getPermutations()) {
      types.push_back(make_shared<TupleType>(make_shared<TypeList>(p)));
    }
    e->type = AmbiguousType(types);
    return false;
  }

  bool visitOperatorExpr(std::shared_ptr<OperatorExpr> e) {
    auto decls = context->filter([e](std::shared_ptr<Decl> d) {
      return d->getName() == e->getLexeme();
    });

    if (decls.isEmpty()) {
      std::stringstream ss;
      ss << "error: "  << e->getLexeme() << " has not been declared" << std::endl;
      throw ss.str();
    } else e->type = decls.getTypes();
    return false;
  }
  bool visitUnaryExpr(std::shared_ptr<UnaryExpr> e) {
    traverseExpr(e->expr);
    AmbiguousTypeList paramType{{e->expr->type}};

    AmbiguousDecl decls = context->filter([e, paramType, this](std::shared_ptr<Decl> d) {
      return d->getName() == e->op->getLexeme()
          && d->as<FuncDecl>()
          && paramType.hasPermutation(d->as<FuncDecl>()->type->params, this->context);
    });

    AmbiguousType types = decls.getTypes().map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
      return t->as<FunctionType>()->returns;
    });;

    if (decls.isEmpty()) {
      std::stringstream ss;
      ss << "error: unary operator " << e->op << " has not been declared for these types "<< std::endl;
      throw ss.str();
    } else if (decls.isAmbiguous()) {
      std::stringstream ss;
      ss << "error: unary operator " << e->op << " is ambiguous for these types "<< std::endl;
      throw ss.str();
    } else {
      e->type = types;
      e->decl = dynamic_pointer_cast<FuncDecl>(decls.get());
    }
    return false;
  }

  bool visitBinaryExpr(std::shared_ptr<BinaryExpr> e) {
    traverseExpr(e->left);
    traverseExpr(e->right);
    AmbiguousTypeList paramType{{e->left->type, e->right->type}};

    AmbiguousDecl decls = context->filter([e, paramType, this](std::shared_ptr<Decl> d) {
      return d->getName() == e->op->getLexeme()
          && d->as<FuncDecl>()
          && paramType.hasPermutation(d->as<FuncDecl>()->type->params, this->context);
    });

    AmbiguousType types = decls.getTypes().map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
      return t->as<FunctionType>()->returns;
    });;

    if (decls.isEmpty()) {
      std::stringstream ss;
      ss << "error: binary operator " << e->op << " has not been declared for these types "<< std::endl;
      throw ss.str();
    }else if (decls.isAmbiguous()) {
      std::stringstream ss;
      ss << "error: binary operator " << e->op << " is ambiguous for these types "<< std::endl;
      throw ss.str();
    } else {
      e->decl = dynamic_pointer_cast<FuncDecl>(decls.get());
      e->type = types;
    }
    return false;
  }

  bool visitFunctionCall(std::shared_ptr<FunctionCall> e) {
    if (e->arguments) {
      traverse(e->arguments);
      AmbiguousTypeList paramType{e->arguments};

      AmbiguousDecl decls = context->filter([e, paramType, this](std::shared_ptr<Decl> d) {
        return d->getName() == e->name->getLexeme()
            && d->as<FuncDecl>()
            && paramType.hasPermutation(d->as<FuncDecl>()->type->params, this->context);
      });

      AmbiguousType type = decls.getTypes().map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
        return t->as<FunctionType>()->returns;
      });;

      if (decls.isEmpty()) {
        std::stringstream ss;
        ss << "error: function " << e->name->getLexeme() << " has not been declared for these types "<< std::endl;
        for (auto decl: context->local()) {
          ss << "how about " << *decl << " ?" << std::endl;
        }
        throw ss.str();
      } else if (decls.isAmbiguous()) {
        std::stringstream ss;
        ss << "error: function " << e->name->getLexeme() << " is ambiguous "<< std::endl;
        throw ss.str();
      } else {
        e->type = type;
        e->decl = dynamic_pointer_cast<FuncDecl>(decls.get());
      }
    } else {
      AmbiguousDecl decls = context->filter([e](std::shared_ptr<Decl> d) {
        return d->getName() == e->name->getLexeme()
            && d->as<FuncDecl>()
            && d->as<FuncDecl>()->type->params == nullptr;
      });

      AmbiguousType types = decls.getTypes().map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
        return t->as<FunctionType>()->returns;
      });;

      if (decls.isEmpty()) {
        std::stringstream ss;
        ss << "error: function " << e->name->getLexeme() << " has not been declared for these types "<< std::endl;
        for (auto decl: context->local()) {
          ss << "how about " << *decl << " ?" << std::endl;
        }
        throw ss.str();
      } else if (decls.isAmbiguous()) {
        std::stringstream ss;
        ss << "error: function " << e->name->getLexeme() << " is ambiguous "<< std::endl;
        throw ss.str();
      } else {
        e->type = types;
        e->decl = dynamic_pointer_cast<FuncDecl>(decls.get());
      }
    }
    return false;
  }

  bool visitExprStmt(std::shared_ptr<ExprStmt> s) {
    traverseExpr(s->expr);
    if (s->expr->type.isSingleton()) {
      auto type = s->expr->type.get();
      auto fund = context->getFundamentalType(type);
    } else {
      std::stringstream ss;
      ss << "error: ambiguous expression " << std::endl << s->expr->type;
      throw ss.str();
    }
    return false;
  }

  bool visitCompoundStmt(std::shared_ptr<CompoundStmt> s) {
    s->context = std::make_shared<DeclarationContext>();
    s->context->setParent(context);
    context = s->context.get();
    traverse(s->list);
    context = context->getParent();
    return false;
  }

  bool visitTypeAlias(std::shared_ptr<TypeAlias> d) {
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

  bool visitFuncDecl(std::shared_ptr<FuncDecl> d) {
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
      if (!d->stmt->returns()) throw std::string("error: doesn't return\n");
    }

    context = context->getParent();
    return false;
  }

  bool visitReturnStmt(std::shared_ptr<ReturnStmt> s) {
    if (!returns) {
      std::stringstream ss;
      ss << "error: you can't return here dipshit " << std::endl;
      throw ss.str();
    } else if (returns && !s->expr){
      std::stringstream ss;
      ss << "error: you gotta return something dipshit " << std::endl;
      throw ss.str();
    } else {
      traverseExpr(s->expr);

      if (s->expr->type.isEmpty()) {
        std::stringstream ss;
        ss << "error: you're returning the wrong type dipshit " << this->returns << " vs " << s->expr->type << std::endl;
        throw ss.str();
      } else if (s->expr->type.isAmbiguous()) {
        std::stringstream ss;
        ss << "error: this is ambiguous you worthless dirtbag" << std::endl;
        throw ss.str();
      }
    }
    return false;
  }

  bool visitLetDecl(std::shared_ptr<LetDecl> d) {
    if (d->expr) traverseExpr(d->expr);
    d->setContext(context);
    if (context->hasLocal(d)) {
      std::stringstream ss;
      ss << "error: redeclaration of " << *d << std::endl;
      throw ss.str();
    }  else {
      context->add(d);
    }
    return false;
  }

  bool visitParamDecl(std::shared_ptr<ParamDecl> d) {
    auto var = std::make_shared<VarDecl>(d->secondary->token, d->type, d->default_value);
    context->add(var);
    return false;
  }

  bool visitVarDecl(std::shared_ptr<VarDecl> d) {
    if (d->expr) traverseExpr(d->expr);
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
};

#endif
