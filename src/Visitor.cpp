#include "Visitor.h"
#include <iostream>
#include "ErrorReporter.h"

void Visitor::visit(IntLiteral* i) {}
void Visitor::visit(DoubleLiteral* t) {}
void Visitor::visit(StringLiteral* t) {}
void Visitor::visit(Identifier* t) {}
void Visitor::visit(Type* t) {}
void Visitor::visit(Operator* t) {}
void Visitor::visit(BinaryExpr* t) {
  t->left->accept(*this);
  if (t->op) {
    t->op->accept(*this);
    t->right->accept(*this);
  }
}
void Visitor::visit(StmtList* t) {
  t->stmt->accept(*this);
  if (t->next != nullptr) {
    t->next->accept(*this);
  }
}
void Visitor::visit(BlockStmt* t) {
  if (t->stmts) {
    t->stmts->accept(*this);
  }
}
void Visitor::visit(VarDecl* t) {
  t->type->accept(*this);
  t->name->accept(*this);
  if (t->value) {
    t->value->accept(*this);
  }
}
void Visitor::visit(ExprList* t) {
  t->stmt->accept(*this);
  if (t->next != nullptr) {
    t->next->accept(*this);
  }
}
void Visitor::visit(FunctionCall* t) {
  t->name->accept(*this);
  if (t->arguments) {
    t->arguments->accept(*this);
  }
}
void Visitor::visit(FuncDecl* t) {
  t->retType->accept(*this);
  t->name->accept(*this);
  if (t->params) {
    t->params->accept(*this);
  }
  t->stmt->accept(*this);
}
void Visitor::visit(IfStmt* t) {
  t->cond->accept(*this);
  t->stmt->accept(*this);
}
void Visitor::visit(WhileStmt* t) {
  t->cond->accept(*this);
  t->stmt->accept(*this);
}

void Visitor::visit(ExprStmt* t) {
  t->expr->accept(*this);
}
void Visitor::visit(ReturnStmt* t) {
  t->expr->accept(*this);
}

void Visitor::visit(Program* t) {
  t->block->accept(*this);
}


class PrintParamList: public Visitor {
  void visit(StmtList* t) {
    t->stmt->accept(*this);
    if (t->next != nullptr) {
      std::cout << ", ";
      t->next->accept(*this);
    }
  }
  void visit(VarDecl* t) {
    PrintVisitor p;
    t->type->accept(p);
    std::cout << " ";
    t->name->accept(p);
  }
};

class PrintFuncDecl: public Visitor {
  void visit(FuncDecl* t) {
    PrintVisitor p;
    PrintParamList param;
    t->retType->accept(p);
    std::cout << " ";
    t->name->accept(p);
    std::cout << "(";
    if (t->params) {
      t->params->accept(param);
    }
    std::cout << ")";
    std::cout << ";" << std::endl;
  }
} printFuncDecl;

class PrintFuncDefn: public Visitor {
  void visit(FuncDecl* t) {
    PrintVisitor p;
    t->retType->accept(p);
    std::cout << " ";
    t->name->accept(p);
    std::cout << "(";
    if (t->params != nullptr) {
      PrintParamList printParam;
      t->params->accept(printParam);
    }
    std::cout << ")";
    t->stmt->accept(p);
  }
} printFuncDefn;


void PrintVisitor::visit(IntLiteral* t) {
  std::cout << t->token.lexeme;
}
void PrintVisitor::visit(DoubleLiteral* t) {
  std::cout << t->token.lexeme;
}
void PrintVisitor::visit(StringLiteral* t) {
  std::cout << t->token.lexeme;
}
void PrintVisitor::visit(Identifier* t) {
  std::cout << t->token.lexeme;
}
void PrintVisitor::visit(Type* t) {
  if (t->token.lexeme == "Int") {
    std::cout << "int";
  } else if (t->token.lexeme == "Double") {
    std::cout << "double";
  } else {
    std::cout << t->token.lexeme;
  }
}
void PrintVisitor::visit(Operator* t) {
  std::cout << t->token.lexeme;
}
void PrintVisitor::visit(BlockStmt* t) {
  std::cout << "{\n";
  if (t->stmts) {
    t->stmts->accept(*this);
  }
  std::cout << "}\n";
}
void PrintVisitor::visit(VarDecl* t) {
  t->type->accept(*this);
  std::cout << " ";
  t->name->accept(*this);
  if (t->value) {
    std::cout << "=";
    t->value->accept(*this);
  }
  std::cout << ";" << std::endl;
}
void PrintVisitor::visit(FunctionCall* t) {
  t->name->accept(*this);
  std::cout << "(";
  if (t->arguments) {
    t->arguments->accept(*this);
  }
  std::cout << ")";
}
void PrintVisitor::visit(ExprList* t) {
  t->stmt->accept(*this);
  if (t->next != nullptr) {
    std::cout << ", ";
    t->next->accept(*this);
  }
}
void PrintVisitor::visit(FuncDecl* t) {}
void PrintVisitor::visit(IfStmt* t) {
  std::cout << "if ";
  std::cout << "(";
  t->cond->accept(*this);
  std::cout << ")";
  t->stmt->accept(*this);
}
void PrintVisitor::visit(WhileStmt* t) {
  std::cout << "while ";
  std::cout << "(";
  t->cond->accept(*this);
  std::cout << ")";
  t->stmt->accept(*this);
}
void PrintVisitor::visit(ExprStmt* t) {
  t->expr->accept(*this);
  std::cout << ";" << std::endl;
}
void PrintVisitor::visit(ReturnStmt* t) {
  std::cout << "return ";
  t->expr->accept(*this);
  std::cout << ";" << std::endl;
}
void PrintVisitor::visit(Program* t) {
  std::cout << "#include <stdio.h>" << std::endl;

  if (t->block->stmts != nullptr) {
    PrintFuncDecl funcDecl;
    PrintFuncDefn funcDefn;
    t->block->stmts->accept(funcDecl);
    t->block->stmts->accept(funcDefn);
    std::cout << "int main() {" << std::endl;
    t->block->stmts->accept(*this);
    std::cout << "return 0;\n}" << std::endl;

  }
}
