#ifndef SEMA_INTERPRETER_H
#define SEMA_INTERPRETER_H

#include "AST/ASTWalker.h"
#include "AST/AmbiguousDecl.h"
#include "Driver/GlobalContext.h"
#include "Sema/RuntimeStack.h"

#include <stack>

class Interpreter : public ASTWalker {
  RuntimeStack stack;

  bool visitStringExpr(std::shared_ptr<StringExpr> e) {
    stack.push(e);
    return false;
  }

  bool visitIntegerExpr(std::shared_ptr<IntegerExpr> e) {
    stack.push(e);
    return false;
  }

  bool visitDoubleExpr(std::shared_ptr<DoubleExpr> e) {
    stack.push(e);
    return false;
  }

  bool visitBoolExpr(std::shared_ptr<BoolExpr> e) {
    stack.push(e);
    return false;
  }

  bool visitTupleExpr(std::shared_ptr<TupleExpr> e) {
    if (e->list) {
      traverse(e->list);
      std::vector<std::shared_ptr<Expr>> elements;
      for (int i=0; i<e->list->size(); i++) {
        elements.push_back(stack.pop());
      }
      std::reverse(elements.begin(), elements.end());
      auto tuple = TupleExpr::make(elements);
      tuple->type = e->type;
      stack.push(tuple);
      return false;
    }
    return false;
  }

  bool visitIdentifierExpr(std::shared_ptr<IdentifierExpr> e) {
    stack.push(stack.get(e->decl->location));
    return false;
  }

  bool visitUnaryExpr(std::shared_ptr<UnaryExpr> s) {
    if (s->decl && s->decl->func) {
      traverse(s->expr);
      s->decl->func(stack);
      stack.move(-1);
      stack.push(stack.popTemp());
    } else {
      traverse(s->expr);
      traverse(s->decl->stmt);
      stack.move(-1);
      stack.push(stack.popTemp());
    }
    return false;
  }

  bool visitBinaryExpr(std::shared_ptr<BinaryExpr> s) {
    if (s->decl && s->decl->func) {
      traverse(s->left);
      traverse(s->right);
      s->decl->func(stack);
      stack.move(-2);
      stack.push(stack.popTemp());
    } else {
      traverse(s->left);
      traverse(s->right);
      traverse(s->decl->stmt);
      stack.move(-2);
      stack.push(stack.popTemp());
    }
    return false;
  }

  bool visitFunctionCall(std::shared_ptr<FunctionCall> s) {
    if (s->decl && s->decl->func) {
      traverse(s->arguments);
      s->decl->func(stack);
      stack.move(-s->arguments->size());
      stack.push(stack.popTemp());
    } else if (s->decl){
      traverse(s->arguments);
      traverse(s->decl->stmt);
      stack.move(-s->arguments->size());
      stack.push(stack.popTemp());
    } else {
      throw std::string("error: can't compute missing function");
    }
    return false;
  }

  bool visitReturnStmt(std::shared_ptr<ReturnStmt> s) {
    traverse(s->expr);
    auto ret = stack.pop();
    stack.pushTemp(ret);
    return false;
  };

  bool visitExprStmt(std::shared_ptr<ExprStmt> s) {
    traverse(s->expr);
    std::cout << stack.top()->type << ": " << stack.pop() << std::endl;
    return false;
  };

  bool visitVarDecl(std::shared_ptr<VarDecl> s) {
    traverse(s->expr);
    return false;
  }

  bool visitLetDecl(std::shared_ptr<LetDecl> s) {
    traverse(s->expr);
    return false;
  }

  bool visitFuncDecl(std::shared_ptr<FuncDecl> s) {
    return false;
  }

};
#endif
