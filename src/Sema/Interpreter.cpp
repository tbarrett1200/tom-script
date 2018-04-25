#include "Sema/Interpreter.h"

bool Interpreter::visitStringExpr(std::shared_ptr<StringExpr> e) {
  stack.push(e);
  return false;
}

bool Interpreter::Interpreter::visitIntegerExpr(std::shared_ptr<IntegerExpr> e) {
  stack.push(e);
  return false;
}

bool Interpreter::visitDoubleExpr(std::shared_ptr<DoubleExpr> e) {
  stack.push(e);
  return false;
}

bool Interpreter::visitBoolExpr(std::shared_ptr<BoolExpr> e) {
  stack.push(e);
  return false;
}

bool Interpreter::visitTupleExpr(std::shared_ptr<TupleExpr> e) {
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

bool Interpreter::visitIdentifierExpr(std::shared_ptr<IdentifierExpr> e) {
  stack.push(stack.get(e->decl->location));
  return false;
}

bool Interpreter::visitUnaryExpr(std::shared_ptr<UnaryExpr> s) {
  // function pre-call
  traverse(s->expr);
  stack.push(stack.getBase());
  stack.setBase(std::make_shared<StackReference>(stack.size()-1));

  if (s->op->decl && s->op->decl->func) {
    s->op->decl->func(stack);
  } else {
    traverse(s->op->decl->stmt);
  }

  // function post-call
  stack.setBase(std::dynamic_pointer_cast<StackReference>(stack.pop()));
  stack.offset(-1);
  stack.push(stack.getResult());
  return false;
}

bool Interpreter::visitBinaryExpr(std::shared_ptr<BinaryExpr> s) {
  // function pre-call
  traverse(s->right);
  traverse(s->left);
  stack.push(stack.getBase());
  stack.setBase(std::make_shared<StackReference>(stack.size()-1));

  if (s->op->decl && s->op->decl->func) {
    s->op->decl->func(stack);
  } else {
    traverse(s->op->decl->stmt);
  }

  // function post-call
  stack.setBase(std::dynamic_pointer_cast<StackReference>(stack.pop()));
  stack.offset(-2);
  stack.push(stack.getResult());
  return false;
}

bool Interpreter::visitFunctionCall(std::shared_ptr<FunctionCall> s) {
  // function pre-call
  if (s->arguments) traverse(s->arguments->reverse());
  stack.push(stack.getBase());
  stack.setBase(std::make_shared<StackReference>(stack.size()-1));

  if (s->decl && s->decl->func) {
    s->decl->func(stack);
  } else {
    traverse(s->decl->stmt);
  }

  // function post-call
  stack.setBase(std::dynamic_pointer_cast<StackReference>(stack.pop()));
  if (s->arguments) stack.offset(-s->arguments->size());
  stack.push(stack.getResult());
  return false;
}

bool Interpreter::visitReturnStmt(std::shared_ptr<ReturnStmt> s) {
  traverse(s->expr);
  auto ret = stack.pop();
  stack.setResult(ret);
  return false;
};

bool Interpreter::visitExprStmt(std::shared_ptr<ExprStmt> s) {
  traverse(s->expr);
  if (*s->expr->type != *Parser::makeType("Void")) {
    std::cout << stack.top()->type << ": " << stack.top() << std::endl;
  }
  stack.pop();
  return false;
};

bool Interpreter::visitConditionalStmtList(std::shared_ptr<ConditionalStmtList> s) {
  if (!s->element) return false;

  if (s->element->condition) {
    traverse(s->element->condition);
    if (stack.top()->as<BoolExpr>()->getLexeme() == "true") {
      stack.pop();
      traverse(s->element->stmt);
    } else if (s->list){
      stack.pop();
      traverse(s->list);
    } else {
      stack.pop();
    }
  } else {
    traverse(s->element->stmt);
  }
  return false;
};

bool Interpreter::visitVarDecl(std::shared_ptr<VarDecl> s) {
  traverse(s->expr);
  return false;
}

bool Interpreter::visitLetDecl(std::shared_ptr<LetDecl> s) {
  traverse(s->expr);
  return false;
}

bool Interpreter::visitFuncDecl(std::shared_ptr<FuncDecl> s) {
  return false;
}
