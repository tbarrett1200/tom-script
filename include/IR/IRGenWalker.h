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
#include "llvm/IR/Verifier.h"

#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"

class LLVMTransformer {
private:
  llvm::LLVMContext& fContext;
  llvm::Module* fModule;
  llvm::BasicBlock *fCurrentBlock;

public:
  LLVMTransformer(llvm::LLVMContext& context, llvm::Module* module) : fContext{context} {
    fModule = module;
  }

  llvm::FunctionType* transformFunctionType(const FunctionType &type) {
    if (type.params.size() == 0) {
      return llvm::FunctionType::get(transformType(*type.returns), false);
    } else {
      throw std::logic_error("functions with parameters not yet supported");
    }
  }

  llvm::Type* transformType(const Type &type) {
    if (type.as<IntegerType>()) {
      return llvm::Type::getInt64Ty(fContext);
    } else if (type.as<DoubleType>()) {
      return llvm::Type::getDoubleTy(fContext);
    } else {
      throw std::logic_error("only integer types are currently supported");
    }
  }

  llvm::Function* transformFunction(const FuncDecl &func) {
    llvm::FunctionType* type = transformFunctionType(*std::dynamic_pointer_cast<FunctionType>(func.getType()));
    llvm::Function* function = llvm::Function::Create(type, llvm::Function::ExternalLinkage, func.getName(), fModule);
    fCurrentBlock = llvm::BasicBlock::Create(fContext, "entry", function);
    llvm::IRBuilder<> builder{fCurrentBlock};
    for (auto stmt: func.getBlockStmt()->list) {
      if (std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
        builder.CreateRet(transformExpr(*std::dynamic_pointer_cast<ReturnStmt>(stmt)->expr));
      } else {
        throw std::logic_error("only return statements are currently supported");
      }
    }
    return function;
  }

  llvm::Value* transformExpr(const Expr& expr) {
    if (dynamic_cast<const IntegerExpr*>(&expr)) {
      return llvm::ConstantInt::get(transformType(*expr.getType()), (uint64_t)(dynamic_cast<const IntegerExpr&>(expr).getInt()), true);
    } else if (dynamic_cast<const DoubleExpr*>(&expr)) {
      return llvm::ConstantFP::get(transformType(*expr.getType()), (dynamic_cast<const DoubleExpr&>(expr).getDouble()));
    } else if (dynamic_cast<const BinaryExpr*>(&expr)) {
      return transformBinaryExpr(dynamic_cast<const BinaryExpr&>(expr));
    }  else {
      throw std::logic_error("only integer expressions currently supported");
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
      } else {
        throw std::logic_error("error: binary expression of this type not implemented");
      }
    } else {
      throw std::logic_error("only integer and double operations currently supported");
    }
  }
};

#endif
