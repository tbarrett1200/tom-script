#include "Sema/ScopeBuilder.h"
#include "Sema/TypeChecker.h"
#include "Sema/BuiltinDecl.h"
#include "Sema/TypeResolver.h"

#include "Basic/CompilerException.h"

#include "AST/DeclContext.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/Expr.h"

void ScopeBuilder::buildGlobalScope() {
  DeclContext* global_context = DeclContext::getGlobalContext();
  global_context->addDecl(&BuiltinDecl::add_int);
  global_context->addDecl(&BuiltinDecl::sub_int);
  global_context->addDecl(&BuiltinDecl::mul_int);
  global_context->addDecl(&BuiltinDecl::div_int);
  global_context->addDecl(&BuiltinDecl::mod_int);
  global_context->addDecl(&BuiltinDecl::neg_int);

  global_context->addDecl(&BuiltinDecl::equ_int);
  global_context->addDecl(&BuiltinDecl::ne_int);
  global_context->addDecl(&BuiltinDecl::lt_int);
  global_context->addDecl(&BuiltinDecl::lte_int);
  global_context->addDecl(&BuiltinDecl::gt_int);
  global_context->addDecl(&BuiltinDecl::gte_int);

  global_context->addDecl(&BuiltinDecl::add_dbl);
  global_context->addDecl(&BuiltinDecl::sub_dbl);
  global_context->addDecl(&BuiltinDecl::mul_dbl);
  global_context->addDecl(&BuiltinDecl::div_dbl);
  global_context->addDecl(&BuiltinDecl::mod_dbl);

  global_context->addDecl(&BuiltinDecl::equ_dbl);
  global_context->addDecl(&BuiltinDecl::ne_dbl);
  global_context->addDecl(&BuiltinDecl::lt_dbl);
  global_context->addDecl(&BuiltinDecl::lte_dbl);
  global_context->addDecl(&BuiltinDecl::gt_dbl);
  global_context->addDecl(&BuiltinDecl::gte_dbl);
  global_context->addDecl(&BuiltinDecl::neg_dbl);

  global_context->addDecl(&BuiltinDecl::int_to_double);
  global_context->addDecl(&BuiltinDecl::double_to_int);
}

void ScopeBuilder::buildCompilationUnitScope(CompilationUnit &unit) {
  buildGlobalScope();
  DeclContext* unitContext = unit.getDeclContext();
  unitContext->setParentContext(DeclContext::getGlobalContext());
  for (auto &stmt: unit.stmts()) {
    if (DeclStmt *decl_stmt = dynamic_cast<DeclStmt*>(stmt.get())) {
      Decl* decl = decl_stmt->getDecl();
      TypeResolver{*unitContext}.resolve(*decl->getType());
      if (FuncDecl *funcDecl = dynamic_cast<FuncDecl*>(decl)) {
        decl->setParentContext(unitContext);
        unitContext->addDecl(decl);
        buildFuncDeclScope(*funcDecl);
      } else {
        buildStmtScope(*stmt, unitContext);
      }
    }
  }
}



void ScopeBuilder::buildDeclScope(class Decl& decl) {

  TypeResolver{*decl.getDeclContext()}.resolve(*decl.getType());

  switch(decl.getKind()) {
    case Decl::Kind::LetDecl:
      buildLetDeclScope(static_cast<LetDecl&>(decl));
      break;
    case Decl::Kind::VarDecl:
      buildVarDeclScope(static_cast<VarDecl&>(decl));
      break;
    case Decl::Kind::UninitializedVarDecl:
      buildUninitializedVarDeclScope(static_cast<UninitializedVarDecl&>(decl));
      break;
    case Decl::Kind::BasicDecl:
      buildBasicDeclScope(static_cast<BasicDecl&>(decl));
      break;
    case Decl::Kind::TypeAlias:
      buildTypeAliasScope(static_cast<TypeAlias&>(decl));
      break;
    case Decl::Kind::ParamDecl:
      buildParamDeclScope(static_cast<ParamDecl&>(decl));
      break;
    case Decl::Kind::FuncDecl:
      buildFuncDeclScope(static_cast<FuncDecl&>(decl));
      break;
    case Decl::Kind::StructDecl:
      buildStructDeclScope(static_cast<StructDecl&>(decl));
      break;
  }
}

void ScopeBuilder::buildLetDeclScope(LetDecl& decl) {
  if (Expr *expr = &decl.getExpr()) {
    TypeChecker{decl.getDeclContext()}.checkExpr(*expr);

    if (decl.getType()->getKind() == Type::Kind::ReferenceType) {
      const ReferenceType *ref_type = dynamic_cast<const ReferenceType*>(decl.getType());
      if (ref_type->getReferencedType() == expr->getType()) {
        if (expr->isLeftValue()) return;
        else {
          std::stringstream ss;
          ss << decl.getName() << " is declared as a`";
          ss << decl.getType()->toString() << "` but initialized as r-value`";
          ss << expr->getType()->toString() << "`";
          throw CompilerException(nullptr, ss.str());
        }
      }
    }

    if (decl.getType() != expr->getType()) {
      std::stringstream ss;
      ss << decl.getName() << " is declared as `";
      ss << decl.getType()->toString() << "` but initialized as `";
      ss << expr->getType()->toString() << "`";
      throw CompilerException(nullptr, ss.str());
    }
  }
}

void ScopeBuilder::buildVarDeclScope(VarDecl& decl) {
  if (Expr *expr = &decl.getExpr()) {
    TypeChecker{decl.getDeclContext()}.checkExpr(*expr);

    if (decl.getType()->getKind() == Type::Kind::ReferenceType) {
      const ReferenceType *ref_type = dynamic_cast<const ReferenceType*>(decl.getType());
      if (ref_type->getReferencedType() == expr->getType()) {
        if (expr->isLeftValue()) return;
        else {
          std::stringstream ss;
          ss << decl.getName() << " is declared as a`";
          ss << decl.getType()->toString() << "` but initialized as r-value`";
          ss << expr->getType()->toString() << "`";
          throw CompilerException(nullptr, ss.str());
        }
      }
    }

    if (decl.getType()->getCanonicalType() != expr->getType()->getCanonicalType()) {
      std::stringstream ss;
      ss << decl.getName() << " is declared as `";
      ss << decl.getType()->toString() << "` but initialized as `";
      ss << expr->getType()->toString() << "`";
      throw CompilerException(nullptr, ss.str());
    }
  }
}

void ScopeBuilder::buildUninitializedVarDeclScope(UninitializedVarDecl& decl) {
  // no checks to be done
}

void ScopeBuilder::buildTypeAliasScope(TypeAlias& decl) {
  // no checks to be done
}

void ScopeBuilder::buildParamDeclScope(ParamDecl& decl) {
  // no checks to be done
}

void ScopeBuilder::buildFuncDeclScope(FuncDecl& decl) {
  function_ = &decl;
  DeclContext *functionScope = decl.getDeclContext();
  TypeResolver{*functionScope}.resolve(*decl.getType());
  for (auto &param: decl.getParams()) {
    functionScope->addDecl(param.get());
  }

  decl.getBlockStmt().getDeclContext()->setParentContext(functionScope);
  buildCompoundStmtScope(decl.getBlockStmt());
  if (!decl.getBlockStmt().returns()) {
    throw CompilerException(decl.getName().start, "function is not guarenteed to return");
  }
}

void ScopeBuilder::buildBasicDeclScope(BasicDecl& decl) {
  // no checks to be done
}

void ScopeBuilder::buildStructDeclScope(StructDecl& decl) {
  // no checks to be done
}

void ScopeBuilder::buildStmtScope(Stmt& stmt, DeclContext *parent) {
  if (DeclStmt *decl_stmt = dynamic_cast<DeclStmt*>(&stmt)) {
    Decl* decl = decl_stmt->getDecl();
    decl->setParentContext(parent);
    parent->addDecl(decl);
    buildDeclScope(*decl);
  } else if (ExprStmt* expr_stmt = dynamic_cast<ExprStmt*>(&stmt)) {
    TypeChecker{parent}.checkExpr(*expr_stmt->getExpr());
  } else if (WhileLoop *loop = dynamic_cast<WhileLoop*>(&stmt)) {
    loop->setParentContext(parent);
    buildWhileLoopScope(*loop);
  } else if (ReturnStmt *ret_stmt = dynamic_cast<ReturnStmt*>(&stmt)) {
    if (Expr *expr = ret_stmt->getExpr()) {
      TypeChecker{parent}.checkExpr(*expr);
      Type* ret_type = dynamic_cast<const FunctionType*>(function_->getType())->getReturnType();
      if (expr->getType()->getCanonicalType() != ret_type->getCanonicalType()) {
        throw CompilerException(nullptr, "type of returned expression does not match declaration");
      }
    }
  } else if (ConditionalBlock *cond_stmt = dynamic_cast<ConditionalBlock*>(&stmt)) {
    for (auto &stmt: cond_stmt->getStmts()) {
      if (ConditionalStmt *cond_stmt = dynamic_cast<ConditionalStmt*>(stmt.get())) {
        cond_stmt->setParentContext(parent);
        buildConditionalStmtScope(*cond_stmt);
      } else if (CompoundStmt *block_stmt = dynamic_cast<CompoundStmt*>(stmt.get())) {
        block_stmt->setParentContext(parent);
        buildCompoundStmtScope(*block_stmt);
      } else {
        buildStmtScope(*stmt, parent);
      }
    }
  }
}

void ScopeBuilder::buildCompoundStmtScope(CompoundStmt &block) {
  DeclContext *block_scope = block.getDeclContext();
  for (auto &stmt: block.getStmts()) {
    buildStmtScope(*stmt, block_scope);
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
  cond_stmt.getBlock().setParentContext(cond_scope);
  buildCompoundStmtScope(cond_stmt.getBlock());
}
