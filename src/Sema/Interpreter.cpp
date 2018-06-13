#include "Sema/Interpreter.h"
#include "Basic/CompilerException.h"
#include "Basic/SourceCode.h"

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

bool Interpreter::visitListExpr(std::shared_ptr<ListExpr> e) {
  stack.push(e);
  return false;
}

bool Interpreter::visitAccessorExpr(std::shared_ptr<AccessorExpr> e) {
  auto list = std::dynamic_pointer_cast<ListExpr>(stack.get(e->id->decl->location));
  if (e->index->getInt() >= list->data->size()) {
    throw CompilerException(SourceManager::currentFile(), e->getLocation(), CompilerExceptionCategory::Error, "runtime error: index out of bounds");
  } else {
    stack.push((*list->data)[e->index->getInt()]);
  }
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
    tuple->setType(e->getType());
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
  stack.push(std::make_shared<StackPointer>(*stack.getBase()));
  stack.setBase(stack.size()-1);

  if (s->op->decl && s->op->decl->func) {
    s->op->decl->func(stack);
  } else {
    traverse(s->op->decl->stmt);
  }

  // function post-call
  stack.setBase(std::dynamic_pointer_cast<StackPointer>(stack.top())->location);
  stack.offset(1);
  stack.push(stack.getResult());
  return false;
}

bool Interpreter::visitBinaryExpr(std::shared_ptr<BinaryExpr> s) {
  traverse(s->right);

  if (s->op->group.assignment) {
    if (!s->left->isLeftValue()) {
      throw CompilerException(s->getLocation(), "error: can not assign to a r-value\n");
    } else if (std::dynamic_pointer_cast<IdentifierExpr>(s->left)) {
      stack.set(std::dynamic_pointer_cast<IdentifierExpr>(s->left)->decl->location, stack.top());
    } else if (std::dynamic_pointer_cast<AccessorExpr>(s->left)) {
      auto accessor = std::dynamic_pointer_cast<AccessorExpr>(s->left);
      auto list = std::dynamic_pointer_cast<ListExpr>(stack.get(accessor->id->decl->location));
      (*list->data)[accessor->index->getInt()] = stack.top();
    } else {
      throw CompilerException(s->getLocation(), "error: this type of l-value is not yet implemented\n");
    }
    return false;
  }

  // function pre-call
  traverse(s->left);

  stack.push(std::make_shared<StackPointer>(*stack.getBase()));
  stack.setBase(stack.size()-1);

  if (s->op->decl && s->op->decl->func) {
    s->op->decl->func(stack);
  } else {
    traverse(s->op->decl->stmt);
  }

  // function post-call
  stack.setBase(std::dynamic_pointer_cast<StackPointer>(stack.pop())->location);
  stack.offset(2);
  stack.push(stack.getResult());
  return false;
}

bool Interpreter::visitFunctionCall(std::shared_ptr<FunctionCall> s) {
  // function pre-call
  if (s->arguments) traverse(s->arguments->reverse());
  stack.push(std::make_shared<StackPointer>(*stack.getBase()));
  stack.setBase(stack.size()-1);

  if (s->decl && s->decl->func) {
    s->decl->func(stack);
  } else {
    traverse(s->decl->stmt);
  }

  // function post-call
  stack.setBase(std::dynamic_pointer_cast<StackPointer>(stack.pop())->location);
  if (s->arguments) stack.offset(s->arguments->size());
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
  if (*s->expr->getType() != *Parser::makeType("Void")) {
    std::cout << stack.top()->getType() << ": " << stack.top() << std::endl;
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

bool Interpreter::visitWhileLoop(std::shared_ptr<WhileLoop> s) {
  if (s->condition) {
    while(true) {
      traverse(s->condition);
      if (stack.top()->as<BoolExpr>()->getBool()) {
        stack.pop();
        traverse(s->stmt);
      } else {
        break;
      }
    }
  }
  return false;
}

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
