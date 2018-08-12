#ifndef IR_GEN_WALKER
#define IR_GEN_WALKER

/*
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
  llvm::BasicBlock *fCurrentBlock;

  DeclContext* currentContext;

  std::map<std::string, std::shared_ptr<VariableValue>> fNamedValues;

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


  void transformReturnStmt(const ReturnStmt& stmt) {
    llvm::IRBuilder<> builder{fCurrentBlock};
    builder.CreateRet(transformExpr(*stmt.expr));
  }

  void transformDeclStmt(const DeclStmt& declStmt) {
    llvm::IRBuilder<> builder{fCurrentBlock};
    Decl* decl = dynamic_cast<Decl*>(declStmt.decl.get());
    if (dynamic_cast<LetDecl*>(decl)) {
      LetDecl *letDecl = dynamic_cast<LetDecl*>(decl);
      llvm::Value *v = transformExpr(*letDecl->getExpr());
      fNamedValues[letDecl->getName()] = std::make_shared<VariableValue>(false, v, nullptr);
    } else if (dynamic_cast<VarDecl*>(decl)) {
      VarDecl *varDecl = dynamic_cast<VarDecl*>(decl);
      llvm::AllocaInst *alloca = builder.CreateAlloca(transformType(*varDecl->getType()), 0, varDecl->getName());
      builder.CreateStore(transformExpr(*varDecl->getExpr()), alloca);
      fNamedValues[varDecl->getName()] = std::make_shared<VariableValue>(true, nullptr, alloca);;
    } else {
      throw std::logic_error("only let and var declarations are currently supported");
    }
  }

  void transformExprStmt(const ExprStmt& exprStmt) {
    llvm::IRBuilder<> builder{fCurrentBlock};
    const Expr* expr = dynamic_cast<const Expr*>(exprStmt.expr.get());
    if (dynamic_cast<const BinaryExpr*>(expr)) {
      const BinaryExpr* binExpr = dynamic_cast<const BinaryExpr*>(expr);
      if (binExpr->getOperator() == "=") {
        if (dynamic_cast<const IdentifierExpr*>(binExpr->left.get())) {
          const IdentifierExpr *identifier = dynamic_cast<const IdentifierExpr*>(binExpr->left.get());
          llvm::AllocaInst *alloca = fNamedValues[identifier->token.lexeme]->fAlloca;
          if (alloca) {
            builder.CreateStore(transformExpr(*binExpr->right), alloca);
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

  void transformWhileLoop(const WhileLoop& whileLoop) {
    llvm::IRBuilder<> builder{fCurrentBlock};

    // create a basic block and break unconditionally on it
    llvm::BasicBlock *loopCondition = llvm::BasicBlock::Create(fContext, "loop_cond", funcContext);
    builder.CreateBr(loopCondition);

    // create a basic block for loop body - will modify fCurrentBlock
    llvm::BasicBlock *loopBody = transformStmtList(funcContext, whileLoop->stmt->list);

    // create a basic block for loop end.
    llvm::BasicBlock *loopEnd = llvm::BasicBlock::Create(fContext, "loop_end", funcContext);

    // generate conditional code
    fCurrentBlock = loopCondition;
    llvm::IRBuilder<> builder{loopCondition};
    llvm::Value *condition = transformExpr(*whileLoop->condition);
    builder.CreateCondBr(condition, loopBody, loopEnd);

    fCurrentBlock = loopEnd;
  }

  llvm::BasicBlock* transformStmtList(llvm::Function* funcContext, std::vector<std::shared_ptr<Stmt>> list) {
    fCurrentBlock = llvm::BasicBlock::Create(fContext, "entry", funcContext);
    for (auto stmt: list) {
      if (std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
        transformReturnStmt(*std::dynamic_pointer_cast<ReturnStmt>(stmt));
      } else if (std::dynamic_pointer_cast<DeclStmt>(stmt)) {
        transformDeclStmt(*std::dynamic_pointer_cast<DeclStmt>(stmt));
      } else if (std::dynamic_pointer_cast<ExprStmt>(stmt)) {
        transformExprStmt(*std::dynamic_pointer_cast<ExprStmt>(stmt));
      } else if (std::dynamic_pointer_cast<WhileLoop>(stmt)) {
        transformWhileLoop(*std::dynamic_pointer_cast<WhileLoop>(stmt));
      } else {
        throw std::logic_error("only return statements are currently supported");
      }
    }
    return fCurrentBlock;
  }

  llvm::Function* transformFunction(const FuncDecl &func) {
    currentContext = func.getDeclContext();

    llvm::FunctionType* type = transformFunctionType(*std::dynamic_pointer_cast<FunctionType>(func.getType()));
    fFunction = llvm::Function::Create(type, llvm::Function::ExternalLinkage, func.getName(), fModule);

    int index = 0;
    for (auto &arg : function->args()) {
      ParamDecl *param = func.getParams()[index++].get();
      arg.setName(param->getName());
      fNamedValues[param->getName()] = std::make_shared<VariableValue>(false, &arg, nullptr);
    }

    transformStmtList(function, func.getBlockStmt()->list);

    return function;
  }


  llvm::Value* transformFunctionCall(const FunctionCall& call) {
    llvm::IRBuilder<> builder{fCurrentBlock};

    //  return fNamedValues[identifierExpr.getLexeme()];
    llvm::Function *CalleeF = fModule->getFunction(call.name->getLexeme());
    if (!CalleeF) {
      throw CompilerException(call.name->token.getLocation(), "unknown function referenced");
    }

    if (CalleeF->arg_size() != call.arguments.size()) {
      throw CompilerException(call.name->token.getLocation(), "wrong number of parameters");
    }

     std::vector<llvm::Value*> ArgsV;
     for (unsigned i = 0, e = call.arguments.size(); i != e; ++i) {
       ArgsV.push_back(transformExpr(*call.arguments[i]));
       if (!ArgsV.back())
         return nullptr;
     }

     return builder.CreateCall(CalleeF, ArgsV, "calltmp");
  }

  llvm::Value* transformIndentifierExpr(const IdentifierExpr& expr) {
    std::shared_ptr<VariableValue> val = fNamedValues[expr.getLexeme()];
    if (val->fIsAlloca) {
      llvm::IRBuilder<> builder{fCurrentBlock};
      return builder.CreateLoad(val->fAlloca);
    } else {
      return val->fValue;
    }
  }

  llvm::Value* transformExpr(const Expr& expr) {
    if (dynamic_cast<const IntegerExpr*>(&expr)) {
      return llvm::ConstantInt::get(transformType(*expr.getType()), (uint64_t)(dynamic_cast<const IntegerExpr&>(expr).getInt()), true);
    } else if (dynamic_cast<const DoubleExpr*>(&expr)) {
      return llvm::ConstantFP::get(transformType(*expr.getType()), (dynamic_cast<const DoubleExpr&>(expr).getDouble()));
    }  else if (dynamic_cast<const BoolExpr*>(&expr)) {
      return llvm::ConstantInt::get(transformType(*expr.getType()), dynamic_cast<const BoolExpr&>(expr).getBool()?1:0);
    } else if (dynamic_cast<const BinaryExpr*>(&expr)) {
      return transformBinaryExpr(dynamic_cast<const BinaryExpr&>(expr));
    } else if (dynamic_cast<const IdentifierExpr*>(&expr)) {
      return transformIndentifierExpr(dynamic_cast<const IdentifierExpr&>(expr));
    } else if (dynamic_cast<const FunctionCall*>(&expr)) {
      return transformFunctionCall(dynamic_cast<const FunctionCall&>(expr));
    } else {
      throw std::logic_error("unable to transform expr of this type");
    }
  }

  llvm::Value* transformBinaryExpr(const BinaryExpr& expr) {
    llvm::IRBuilder<> builder{fCurrentBlock};
    llvm::Value *lval, *rval;
    lval = transformExpr(*expr.left);
    rval = transformExpr(*expr.right);

    if (expr.getType()->isIntegerType()) {
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
    } else if (expr.getType()->isDoubleType()) {
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
*/
#endif
