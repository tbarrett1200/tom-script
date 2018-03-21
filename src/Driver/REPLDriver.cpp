#include <iostream>
#include <sstream>
#include <string>
#include <stack>

#include "Basic/SourceCode.h"
#include "Parse/Parser.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/AmbiguousType.h"
#include "AST/AmbiguousDecl.h"
#include "AST/ASTWalker.h"
#include "AST/DeclarationContext.h"
#include "Driver/GlobalContext.h"

class Interpreter : public ASTWalker {
public:
  DeclarationContext *context = &globalContext;
  std::stack<std::shared_ptr<Expr>> stack;

  bool visitLabeledExpr(std::shared_ptr<LabeledExpr> e) {
    traverseExpr(e->expr);
    std::vector<std::shared_ptr<Type>> types;
    for (auto type: e->expr->type.types) {
      types.push_back({make_shared<LabeledType>(make_shared<TypeLabel>(e->label->name), type)});
    }
    e->type = {types};
    return false;
  }

  bool visitIntegerExpr(std::shared_ptr<IntegerExpr> e) {
    e->type = {Parser::makeType("Int")};
    return false;
  }

  bool visitStringExpr(std::shared_ptr<StringExpr> e) {
    e->type = {Parser::makeType("String")};
    return false;
  }

  bool visitBoolExpr(std::shared_ptr<BoolExpr> e) {
    e->type = {Parser::makeType("Bool")};
    return false;
  }

  bool visitDoubleExpr(std::shared_ptr<DoubleExpr> e) {
    e->type = {Parser::makeType("Double")};
    return false;
  }

  bool visitIdentifierExpr(std::shared_ptr<IdentifierExpr> e) {
    auto decls = context->filter([e](std::shared_ptr<Decl> d) {
      return d->getName() == e->getLexeme();
    });

    if (decls.isEmpty()) {
      std::cout << "error: no matching operator" << std::endl;
    } else e->type = decls.getTypes();
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
      std::cout << "error: no matching operator" << std::endl;
    } else e->type = decls.getTypes();
    return false;
  }
  bool visitUnaryExpr(std::shared_ptr<UnaryExpr> e) {
    traverseExpr(e->expr);
    AmbiguousTypeList paramType{{e->expr->type}};

    AmbiguousType decls = context->filter([e, paramType](std::shared_ptr<Decl> d) {
      return d->getName() == e->op->getLexeme()
          && d->as<FuncDecl>()
          && paramType.hasPermutation(d->as<FuncDecl>()->type->params);
    }).getTypes().map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
      return t->as<FunctionType>()->returns;
    });;

    if (decls.isEmpty()) {
      std::cout << "error: no matching operator" << std::endl;
    } else e->type = decls;
    return false;
  }

  bool visitBinaryExpr(std::shared_ptr<BinaryExpr> e) {
    traverseExpr(e->left);
    traverseExpr(e->right);
    AmbiguousTypeList paramType{{e->left->type, e->right->type}};
    AmbiguousType decls = context->filter([e, paramType, this](std::shared_ptr<Decl> d) {
      return d->getName() == e->op->getLexeme()
          && d->as<FuncDecl>()
          && paramType.hasPermutation(d->as<FuncDecl>()->type->params, this->context);
    }).getTypes().map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
      return t->as<FunctionType>()->returns;
    });

    if (decls.isEmpty()) {
      std::cout << "error: no matching operator" << std::endl;
    } else e->type = decls;
    return false;
  }

  bool visitFunctionCall(std::shared_ptr<FunctionCall> e) {
    if (e->arguments) {
      traverse(e->arguments);
      AmbiguousTypeList paramType{e->arguments};

      AmbiguousType decls = context->filter([e, paramType](std::shared_ptr<Decl> d) {
        return d->getName() == e->name->getLexeme()
            && d->as<FuncDecl>()
            && paramType.hasPermutation(d->as<FuncDecl>()->type->params);
      }).getTypes().map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
        return t->as<FunctionType>()->returns;
      });;

      if (decls.isEmpty()) {
        std::cout << "error: no matching function" << std::endl;
      } else e->type = decls;
    } else {
      AmbiguousType decls = context->filter([e](std::shared_ptr<Decl> d) {
        return d->getName() == e->name->getLexeme()
            && d->as<FuncDecl>()
            && d->as<FuncDecl>()->type->params == nullptr;
      }).getTypes().map<std::shared_ptr<Type>>([](std::shared_ptr<Type> t){
        return t->as<FunctionType>()->returns;
      });;

      if (decls.isEmpty()) {
        std::cout << "error: no matching function" << std::endl;
      } else e->type = decls;
    }
    return false;
  }

  bool visitExprStmt(std::shared_ptr<ExprStmt> s) {
    traverseExpr(s->expr);
    if (s->expr->type.isSingleton()) {
      auto type = s->expr->type.get();
      auto fund = context->getFundamentalType(type);
      if (*type == *fund) {
        std::cout << type << std::endl;
      } else {
        std::cout << type << " => " << context->getFundamentalType(type) << std::endl;
      }
    }
    return false;
  }

  bool visitTypeAlias(std::shared_ptr<TypeAlias> d) {
    d->setContext(context);
    if (context->has(d)) std::cout << "error: redeclaration of " << d << std::endl;
    else context->add(d);
    return true;
  }

  bool visitFuncDecl(std::shared_ptr<FuncDecl> d) {
    d->setContext(context);
    if (context->has(d)) std::cout << "error: redeclaration of " << *d << std::endl;
    else context->add(d);
    return true;
  }

  bool visitLetDecl(std::shared_ptr<LetDecl> d) {
    if (d->expr) traverseExpr(d->expr);
    d->setContext(context);
    if (context->has(d)) std::cout << "error: redeclaration of " << *d << std::endl;
    else context->add(d);
    return false;
  }

  bool visitVarDecl(std::shared_ptr<VarDecl> d) {
    if (d->expr) traverseExpr(d->expr);
    d->setContext(context);
    if (context->has(d)) std::cout << "error: redeclaration of " << *d << std::endl;
    else context->add(d);
    return false;
  }
};


int main(int argc, char const *argv[]) {
  std::cout << "tom-script (0.1) Enter ':help' for help and ':quit' to quit" << std::endl;
  auto source = SourceCode{};
  auto parser = Parser{&source};
  do {
    if (!parser.token().is(Token::eof)) {
      try {
        auto type = parser.parseStmt();
        Interpreter().traverse(type);
      } catch (std::string s) {
        std::cout << s;
        parser.consumeUntil({Token::new_line, Token::eof});
        parser.consume();
      }
      source.reset();
    }
  } while (!std::cin.eof());
  return 0;
}
