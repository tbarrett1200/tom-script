#include "Sema/ScopeBuilder.h"
#include "Sema/TypeChecker.h"
#include "Sema/BuiltinDecl.h"

#include "Basic/CompilerException.h"

#include "AST/DeclContext.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/Expr.h"

void ScopeBuilder::buildCompilationUnitScope(CompilationUnit &unit) {
  DeclContext* global_context = DeclContext::getGlobalContext();
  global_context->addDecl(&BuiltinDecl::add_int);
  global_context->addDecl(&BuiltinDecl::sub_int);
  global_context->addDecl(&BuiltinDecl::mul_int);

  global_context->addDecl(&BuiltinDecl::assign_int);
  global_context->addDecl(&BuiltinDecl::equ_int);
  global_context->addDecl(&BuiltinDecl::lt_int);
  global_context->addDecl(&BuiltinDecl::lte_int);
  global_context->addDecl(&BuiltinDecl::gt_int);
  global_context->addDecl(&BuiltinDecl::gte_int);

  DeclContext* unitContext = unit.getDeclContext();
  unitContext->setParentContext(global_context);
  for (auto &stmt: unit.getStmts()) {
    if (DeclStmt *decl_stmt = dynamic_cast<DeclStmt*>(stmt.get())) {
      Decl* decl = decl_stmt->getDecl();
      decl->setParentContext(unitContext);
      unitContext->addDecl(decl);

      if (FuncDecl *funcDecl = dynamic_cast<FuncDecl*>(decl)) {
        buildFunctionScope(*funcDecl);
      }
    }
  }
}

void ScopeBuilder::buildFunctionScope(FuncDecl &func) {
  DeclContext *functionScope = func.getDeclContext();
  for (auto &param: func.getParams()) {
    functionScope->addDecl(param.get());
  }
  func.getBlockStmt().getDeclContext()->setParentContext(functionScope);
  buildCompoundStmtScope(func.getBlockStmt());
}

void ScopeBuilder::buildCompoundStmtScope(CompoundStmt &block) {
  DeclContext *block_scope = block.getDeclContext();
  for (auto &stmt: block.getStmts()) {
    if (DeclStmt *decl_stmt = dynamic_cast<DeclStmt*>(stmt.get())) {
      Decl* decl = decl_stmt->getDecl();
      decl->setParentContext(block_scope);
      block_scope->addDecl(decl);
      if (LetDecl *let_decl = dynamic_cast<LetDecl*>(decl)) {
        if (Expr *expr = &let_decl->getExpr()) {
          TypeChecker{block_scope}.checkExpr(*expr);
        }
      } else if (VarDecl *let_decl = dynamic_cast<VarDecl*>(decl)) {
        if (Expr *expr = &let_decl->getExpr()) {
          TypeChecker{block_scope}.checkExpr(*expr);
        }
      }
    } else if (ExprStmt* expr_stmt = dynamic_cast<ExprStmt*>(stmt.get())) {
      TypeChecker{block_scope}.checkExpr(*expr_stmt->getExpr());
    } else if (WhileLoop *loop = dynamic_cast<WhileLoop*>(stmt.get())) {
      loop->setParentContext(block_scope);
      buildWhileLoopScope(*loop);
    } else if (ReturnStmt *ret_stmt = dynamic_cast<ReturnStmt*>(stmt.get())) {
      if (Expr *expr = &ret_stmt->getExpr()) {
        TypeChecker{block_scope}.checkExpr(*expr);
      }
    } else if (ConditionalBlock *cond_stmt = dynamic_cast<ConditionalBlock*>(stmt.get())) {
      for (auto &stmt: cond_stmt->getStmts()) {
        if (ConditionalStmt *cond_stmt = dynamic_cast<ConditionalStmt*>(stmt.get())) {
          cond_stmt->setParentContext(block_scope);
          buildConditionalStmtScope(*cond_stmt);
        } else if (CompoundStmt *block_stmt = dynamic_cast<CompoundStmt*>(stmt.get())) {
          block_stmt->setParentContext(block_scope);
          buildCompoundStmtScope(*block_stmt);
        }
      }
    }
   }
}

void ScopeBuilder::buildWhileLoopScope(class WhileLoop &while_loop) {
  DeclContext *loop_scope = while_loop.getDeclContext();
  // check the condition for the while loop
  Expr* loop_condition = while_loop.getCondition();
  TypeChecker{loop_scope}.checkExpr(*loop_condition);
  if (!loop_condition->getType()->isBooleanType()) {
    throw CompilerException(
      nullptr
    , "error: loop condition must be of Boolean type"
    );
  }
  // if the loop contains a declaration, add it to the scope
  if (LetDecl *let_decl = while_loop.getDeclaration()) {
    loop_scope->addDecl(let_decl);
    let_decl->setParentContext(loop_scope);
  }
  while_loop.getBlock()->setParentContext(loop_scope);
  buildCompoundStmtScope(*while_loop.getBlock());
}

void ScopeBuilder::buildConditionalStmtScope(class ConditionalStmt &cond_stmt) {
  DeclContext *cond_scope = cond_stmt.getDeclContext();
  // if conditional statement is not a 'else' stmt - check its expression
  if (Expr* condition = cond_stmt.getCondition()) {
    TypeChecker{cond_scope}.checkExpr(*condition);
    if (!condition->getType()->isBooleanType()) {
      throw CompilerException(
        nullptr
      , "error: condition must be of Boolean type. got " + condition->getType()->toString() + " instead"
      );
    }
  }
  // if conditional statement contains a declaration, add it to the scope
  if (LetDecl *let_decl = cond_stmt.getDeclaration()) {
    cond_scope->addDecl(let_decl);
    let_decl->setParentContext(cond_scope);
  }
  cond_stmt.getBlock()->setParentContext(cond_scope);
  buildCompoundStmtScope(*cond_stmt.getBlock());
}
