#ifndef IR_GEN_WALKER
#define IR_GEN_WALKER


#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Analysis/Interval.h"

#include "llvm/IR/Verifier.h"

#include "Basic/CompilerException.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"
#include <vector>
#include <map>

class VariableValue {
public:
  bool fIsAlloca;
  llvm::Value* fValue;
  llvm::AllocaInst* fAlloca;
  VariableValue(const VariableValue&) = default;
  VariableValue(bool isAlloca, llvm::Value* value, llvm::AllocaInst *alloca)
  : fIsAlloca{isAlloca}, fValue{value}, fAlloca{alloca} {}
};

class LLVMTransformer {
private:
  llvm::LLVMContext& fContext;
  llvm::Module* fModule;
  llvm::Function* fFunction;
  const DeclContext* currentContext;

  std::map<StringRef, std::shared_ptr<VariableValue>> fNamedValues;

public:
  LLVMTransformer(llvm::LLVMContext& context, llvm::Module* module) : fContext{context} {
    fModule = module;
  }

  llvm::FunctionType* transformFunctionType(const FunctionType &type) {
    if (type.params.size() == 0) {
      return llvm::FunctionType::get(transformType(*type.returns), false);
    } else {
      std::vector<llvm::Type*> paramTypes;
      for (auto paramType: type.params) {
        paramTypes.push_back(transformType(*paramType));
      }
      return llvm::FunctionType::get(transformType(*type.returns), paramTypes, false);
    }
  }

  llvm::Type* transformType(const Type &type) {
    if (type.isIntegerType()) {
      return llvm::Type::getInt64Ty(fContext);
    } if (type.isBooleanType()) {
      return llvm::Type::getInt1Ty(fContext);
    } else if (type.isDoubleType()) {
      return llvm::Type::getDoubleTy(fContext);
    } else {
      throw std::logic_error("only integer, boolean, and double types are currently supported");
    }
  }


  void transformReturnStmt(const ReturnStmt& stmt, llvm::BasicBlock* current_block) {
    llvm::IRBuilder<> builder{current_block};
    builder.CreateRet(transformExpr(stmt.getExpr(), current_block));
  }

  void transformDeclStmt(const DeclStmt& declStmt, llvm::BasicBlock* current_block) {
    llvm::IRBuilder<> builder{current_block};
    const Decl* decl = dynamic_cast<const Decl*>(declStmt.getDecl());
    if (dynamic_cast<const LetDecl*>(decl)) {
      const LetDecl *letDecl = dynamic_cast<const LetDecl*>(decl);
      llvm::Value *v = transformExpr(*letDecl->getExpr(),current_block);
      fNamedValues[letDecl->getName()] = std::make_shared<VariableValue>(false, v, nullptr);
    } else if (dynamic_cast<const VarDecl*>(decl)) {
      const VarDecl *varDecl = dynamic_cast<const VarDecl*>(decl);
      llvm::AllocaInst *alloca = builder.CreateAlloca(transformType(*varDecl->getType()), 0, varDecl->getName().str());
      builder.CreateStore(transformExpr(*varDecl->getExpr(),current_block), alloca);
      fNamedValues[varDecl->getName()] = std::make_shared<VariableValue>(true, nullptr, alloca);;
    } else {
      throw std::logic_error("only let and var declarations are currently supported");
    }
  }

  void transformExprStmt(const ExprStmt& exprStmt, llvm::BasicBlock* current_block) {
    llvm::IRBuilder<> builder{current_block};
    const Expr* expr = dynamic_cast<const Expr*>(exprStmt.getExpr());
    if (dynamic_cast<const BinaryExpr*>(expr)) {
      const BinaryExpr* binExpr = dynamic_cast<const BinaryExpr*>(expr);
      if (binExpr->getOperator() == "=") {
        if (dynamic_cast<const IdentifierExpr*>(&binExpr->getLeft())) {
          const IdentifierExpr *identifier = dynamic_cast<const IdentifierExpr*>(&binExpr->getLeft());
          llvm::AllocaInst *alloca = fNamedValues[identifier->lexeme()]->fAlloca;
          if (alloca) {
            builder.CreateStore(transformExpr(binExpr->getRight(),current_block), alloca);
          } else {
            throw std::logic_error("lvalue is not mutable or not found");
          }
        } else {
          throw std::logic_error("can only assign to lvalue");
        }
      } else {
        throw std::logic_error("only assignment expr stmts currently supported");
      }
    }
  }

  llvm::Function* transformFunction(const FuncDecl &func) {
    currentContext = func.getDeclContext();

    llvm::FunctionType* type = transformFunctionType(*std::dynamic_pointer_cast<FunctionType>(func.getType()));
    fFunction = llvm::Function::Create(type, llvm::Function::ExternalLinkage, func.getName().str(), fModule);

    int index = 0;
    for (auto &arg : fFunction->args()) {
      ParamDecl *param = func.getParams()[index++].get();
      arg.setName(param->getName().str());
      fNamedValues[param->getName()] = std::make_shared<VariableValue>(false, &arg, nullptr);
    }

    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(fContext, "entry", fFunction);

    transformCompoundStmt(*func.getBlockStmt(), entry_block);

    return fFunction;
  }


  llvm::Value* transformFunctionCall(const FunctionCall& call, llvm::BasicBlock* current_block) {
    llvm::IRBuilder<> builder{current_block};

    //  return fNamedValues[identifierExpr.getLexeme()];
    llvm::Function *CalleeF = fModule->getFunction(call.getFunctionName().str());
    if (!CalleeF) {
      throw CompilerException(call.getFunctionName().start, "unknown function referenced");
    }

    if (CalleeF->arg_size() != call.getArguments().size()) {
      throw CompilerException(call.getFunctionName().start, "wrong number of parameters");
    }

     std::vector<llvm::Value*> ArgsV;
     for (unsigned i = 0, e = call.getArguments().size(); i != e; ++i) {
       ArgsV.push_back(transformExpr(*call.getArguments()[i],current_block));
       if (!ArgsV.back())
         return nullptr;
     }

     return builder.CreateCall(CalleeF, ArgsV, "calltmp");
  }

  llvm::Value* transformIndentifierExpr(const IdentifierExpr& expr, llvm::BasicBlock* current_block) {
    std::shared_ptr<VariableValue> val = fNamedValues[expr.lexeme()];
    if (val->fIsAlloca) {
      llvm::IRBuilder<> builder{current_block};
      return builder.CreateLoad(val->fAlloca);
    } else {
      return val->fValue;
    }
  }

  llvm::Value* transformExpr(const Expr& expr, llvm::BasicBlock* current_block) {
    if (dynamic_cast<const IntegerExpr*>(&expr)) {
      return llvm::ConstantInt::get(transformType(*expr.getType()), (uint64_t)(dynamic_cast<const IntegerExpr&>(expr).getInt()), true);
    } else if (dynamic_cast<const DoubleExpr*>(&expr)) {
      return llvm::ConstantFP::get(transformType(*expr.getType()), (dynamic_cast<const DoubleExpr&>(expr).getDouble()));
    }  else if (dynamic_cast<const BoolExpr*>(&expr)) {
      return llvm::ConstantInt::get(transformType(*expr.getType()), dynamic_cast<const BoolExpr&>(expr).getBool()?1:0);
    } else if (dynamic_cast<const BinaryExpr*>(&expr)) {
      return transformBinaryExpr(dynamic_cast<const BinaryExpr&>(expr),current_block);
    } else if (dynamic_cast<const IdentifierExpr*>(&expr)) {
      return transformIndentifierExpr(dynamic_cast<const IdentifierExpr&>(expr),current_block);
    } else if (dynamic_cast<const FunctionCall*>(&expr)) {
      return transformFunctionCall(dynamic_cast<const FunctionCall&>(expr),current_block);
    } else {
      throw std::logic_error("unable to transform expr of this type");
    }
  }

  llvm::BasicBlock* transformCompoundStmt(CompoundStmt& tree, llvm::BasicBlock *current_block) {

    for (auto it = tree.getStmts().begin(); it != tree.getStmts().end(); it++) {
      Stmt* stmt = it->get();
      bool last = std::distance(it, tree.getStmts().end()) == 1;
      if (ReturnStmt* ret_stmt = dynamic_cast<ReturnStmt*>(stmt)) {
        transformReturnStmt(*ret_stmt, current_block);
        return current_block;
      } else if (DeclStmt* decl_stmt = dynamic_cast<DeclStmt*>(stmt)) {
        transformDeclStmt(*decl_stmt, current_block);
      } else if (ExprStmt *expr_stmt = dynamic_cast<ExprStmt*>(stmt)) {
        transformExprStmt(*expr_stmt, current_block);
      } else if (ConditionalBlock* cond_block = dynamic_cast<ConditionalBlock*>(stmt)) {
        current_block = transformConditionalBlock(*cond_block, current_block);
      } else if (WhileLoop *while_loop = dynamic_cast<WhileLoop*>(stmt)) {
        current_block = transformWhileLoop(*while_loop, current_block);
      } else {
        throw std::logic_error("unsupported statement type");
      }
    }
    return current_block;
  }

  llvm::BasicBlock* transformConditionalBlock(
    ConditionalBlock& tree,
    llvm::BasicBlock* current_block
  ) {

    llvm::BasicBlock *if_exit = llvm::BasicBlock::Create(fContext, "if_exit", fFunction);

    // for simplicity of debugging, create seperate cond_case block
    llvm::IRBuilder<> entry_builder{current_block};
    llvm::BasicBlock *if_cond = llvm::BasicBlock::Create(fContext, "if_cond", fFunction);
    entry_builder.CreateBr(if_cond);

    for (auto it = tree.getStmts().begin(); it != tree.getStmts().end(); it++) {
      Stmt* stmt = it->get();
      bool last_block = std::distance(it, tree.getStmts().end()) == 1;


      // the alternative block must be generated in all cases except 'else'
      llvm::BasicBlock *next_block = last_block ? nullptr : llvm::BasicBlock::Create(fContext, "else_if_cond", fFunction);

      if (ConditionalStmt* cond_stmt = dynamic_cast<ConditionalStmt*>(stmt)) {
        transformConditionalStmt(*cond_stmt, if_cond, next_block, if_exit);
        if_cond = next_block;
      } else {
        if_cond->setName("else");
        llvm::BasicBlock *else_exit = transformCompoundStmt(dynamic_cast<CompoundStmt&>(*stmt), if_cond);
        if (!else_exit->getTerminator()) {
          llvm::IRBuilder<> else_exit_builder{else_exit};
          else_exit_builder.CreateBr(if_exit);
        }
      }
    }
    if (llvm::pred_begin(if_exit) == llvm::pred_end(if_exit)) {
      if_exit->removeFromParent();
    }
    return if_exit;
  }

  llvm::BasicBlock* transformWhileLoop(
    WhileLoop& tree,
    llvm::BasicBlock* entry_block
  ) {
    llvm::BasicBlock *cond_block = llvm::BasicBlock::Create(fContext, "loop_cond", fFunction);
    llvm::IRBuilder<> entry_builder{entry_block};
    entry_builder.CreateBr(cond_block);

    llvm::IRBuilder<> cond_builder{cond_block};
    llvm::Value* condition = transformExpr(*tree.getCondition(), cond_block);
    llvm::BasicBlock *loop_block = llvm::BasicBlock::Create(fContext, "loop_body", fFunction);
    llvm::BasicBlock *loop_exit = llvm::BasicBlock::Create(fContext, "loop_exit", fFunction);
    cond_builder.CreateCondBr(condition, loop_block, loop_exit);

    llvm::BasicBlock* loop_body_exit = transformCompoundStmt(*tree.getBlock(), loop_block);
    if (!loop_body_exit->getTerminator()) {
      llvm::IRBuilder<> loop_body_exit_builder{loop_body_exit};
      loop_body_exit_builder.CreateBr(cond_block);
    }
    return loop_exit;
  }


  void transformConditionalStmt(
    ConditionalStmt& tree,
    llvm::BasicBlock* if_cond,
    llvm::BasicBlock* next_block,
    llvm::BasicBlock* if_exit
  ) {
    llvm::IRBuilder<> cond_builder{if_cond};
    llvm::Value* condition = transformExpr(*tree.getCondition(), if_cond);
    llvm::BasicBlock *if_body = llvm::BasicBlock::Create(fContext, "if_body", fFunction);

    if (next_block) {
      cond_builder.CreateCondBr(condition, if_body, next_block);
    } else {
      cond_builder.CreateCondBr(condition, if_body, if_exit);
    }

    llvm::BasicBlock *if_body_exit = transformCompoundStmt(*tree.getBlock(), if_body);
    if (!if_body_exit->getTerminator()) {
      llvm::IRBuilder<> if_body_exit_builder{if_body_exit};
      if_body_exit_builder.CreateBr(if_exit);
    }

  }


  llvm::Value* transformBinaryExpr(const BinaryExpr& expr, llvm::BasicBlock* current_block) {
    llvm::IRBuilder<> builder{current_block};
    llvm::Value *lval, *rval;
    lval = transformExpr(expr.getLeft(), current_block);
    rval = transformExpr(expr.getRight(), current_block);

    if (lval->getType()->isIntegerTy() && rval->getType()->isIntegerTy()) {
      if (expr.getOperator() == "+") {
        return  builder.CreateAdd(lval, rval);
      } else if (expr.getOperator() == "-") {
        return builder.CreateSub(lval, rval);
      } else if (expr.getOperator() == "*") {
        return builder.CreateMul(lval, rval);
      } else if (expr.getOperator() == "/") {
        return builder.CreateSDiv(lval, rval);
      } else if (expr.getOperator() == "%") {
        return builder.CreateSRem(lval, rval);
      } else if (expr.getOperator() == "==") {
        return builder.CreateICmpEQ(lval, rval);
      } else if (expr.getOperator() == "!=") {
        return builder.CreateICmpNE(lval, rval);
      } else if (expr.getOperator() == ">=") {
        return builder.CreateICmpSGE(lval, rval);
      } else if (expr.getOperator() == "<=") {
        return builder.CreateICmpSLE(lval, rval);
      } else if (expr.getOperator() == ">") {
        return builder.CreateICmpSGT(lval, rval);
      } else if (expr.getOperator() == "<") {
        return builder.CreateICmpSLT(lval, rval);
      } else {
        throw std::logic_error("error: binary expression of this type not implemented");
      }
    } else if (lval->getType()->isDoubleTy() && rval->getType()->isDoubleTy()) {
      if (expr.getOperator() == "+") {
        return builder.CreateFAdd(lval, rval);
      } else if (expr.getOperator() == "-") {
        return builder.CreateFSub(lval, rval);
      } else if (expr.getOperator() == "*") {
        return builder.CreateFMul(lval, rval);
      } else if (expr.getOperator() == "/") {
        return builder.CreateFDiv(lval, rval);
      } else if (expr.getOperator() == "==") {
        return builder.CreateFCmpOEQ(lval, rval);
      } else if (expr.getOperator() == "!=") {
        return builder.CreateFCmpONE(lval, rval);
      } else if (expr.getOperator() == ">=") {
        return builder.CreateFCmpOGE(lval, rval);
      } else if (expr.getOperator() == "<=") {
        return builder.CreateFCmpOLE(lval, rval);
      } else if (expr.getOperator() == ">") {
        return builder.CreateFCmpOGT(lval, rval);
      } else if (expr.getOperator() == "<") {
        return builder.CreateFCmpOLT(lval, rval);
      } else {
        throw std::logic_error("error: binary expression of this type not implemented");
      }
    } else if (expr.getType()->isBooleanType()) {
      if (expr.getOperator() == "&&") {
        return builder.CreateAnd(lval, rval);
      } else if (expr.getOperator() == "||") {
        return builder.CreateOr(lval, rval);
      } else {
        throw std::logic_error("error: binary expression of this type not implemented");
      }
    } else {
      throw std::logic_error("only integer and double operations currently supported");
    }
  }
};

#endif
