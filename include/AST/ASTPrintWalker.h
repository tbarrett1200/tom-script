#ifndef AST_XML_PRINT_WALKER
#define AST_XML_PRINT_WALKER

#include "AST/ASTWalker.h"
#include "AST/Decl.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"
#include <ostream>
#include <string>
#include <memory>

void printASTNode(std::ostream& os, TreeElement* m) {
  if (Decl* decl = dynamic_cast<Decl*>(m)) {
    os << "{ \"name\":";
    switch(decl->getKind()) {
      case Decl::Kind::TypeAlias:
        os << "\"alias ";
        break;
      case Decl::Kind::VarDecl:
        os << "\"var ";
        break;
      case Decl::Kind::ParamDecl:
        os << "\"param ";
        break;
      case Decl::Kind::LetDecl:
        os << "\"let ";
        break;
      case Decl::Kind::FuncDecl:
        os << "\"func ";
        break;
    }
    os << decl->getName() << " " << decl->getType()->toString() << "\",";
  } else if (Expr* expr = dynamic_cast<Expr*>(m)) {
    os << "{ \"name\":";
    switch (expr->getKind()) {
      case Expr::Kind::BinaryExpr:
        os << "\"operator "<< dynamic_cast<BinaryExpr*>(m)->getOperator();
        break;
      case Expr::Kind::IntegerExpr:
        os << "\"expr "<< dynamic_cast<IntegerExpr*>(m)->getInt();
        break;
      case Expr::Kind::DoubleExpr:
        os << "\"expr "<< dynamic_cast<DoubleExpr*>(m)->getDouble();
        break;
      case Expr::Kind::FunctionCall:
        os << "\"call "<< dynamic_cast<FunctionCall*>(m)->getFunctionName();
        break;
      case Expr::Kind::IdentifierExpr:
        os << "\"expr "<< dynamic_cast<IdentifierExpr*>(m)->getLexeme();
        break;
      case Expr::Kind::BoolExpr:
        os << "\"expr "<< dynamic_cast<BoolExpr*>(m)->getBool();
        break;
      case Expr::Kind::UnaryExpr:
        os << "\"operator "<< dynamic_cast<UnaryExpr*>(m)->getOperator();
        break;
      default: os << "\"expr";
    }
    os << ": " << expr->getType()->toString() << "\",";
  } else if (CompilationUnit* unit = dynamic_cast<CompilationUnit*>(m)) {
    os << "{ \"name\":";
    os << "\"unit\",";
  } else if (DeclStmt* decl_stmt = dynamic_cast<DeclStmt*>(m)) {
    printASTNode(os, decl_stmt->getDecl());
    return;
  } else if (ExprStmt* expr_stmt = dynamic_cast<ExprStmt*>(m)) {
    printASTNode(os, expr_stmt->getExpr());
    return;
  } else if (CompoundStmt* block_stmt = dynamic_cast<CompoundStmt*>(m)) {
    os << "{ \"name\":";
    os << "\"block\",";
  } else if (ReturnStmt* return_stmt = dynamic_cast<ReturnStmt*>(m)) {
    os << "{ \"name\":";
    os << "\"return\",";
  } else if (ConditionalBlock* conditional_block = dynamic_cast<ConditionalBlock*>(m)) {
    os << "{ \"name\":";
    os << "\"conditional\",";
  } else if (ConditionalStmt* conditional_stmt = dynamic_cast<ConditionalStmt*>(m)) {
    os << "{ \"name\":";
    if (conditional_stmt->getCondition())
    os << "\"if\",";
    else
    os << "\"while\",";
  } else {
    os << "{ \"name\":";
    os << "\"" << typeid(*m).name() << "\",";
  }
  os << "\"children\": [";
  for (auto it = m->getChildren().begin(); it != m->getChildren().end(); it++) {
    printASTNode(os, it->get());
    if (std::distance(it, m->getChildren().end()) != 1) os << ",";
  }
  os << "] }";
};

#endif
