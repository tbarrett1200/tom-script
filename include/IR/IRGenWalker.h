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
  llvm::LLVMContext fContext;
  std::unique_ptr<llvm::Module> fModule;
  llvm::IRBuilder<> fBuilder{fContext};

public:
  LLVMTransformer(std::string moduleName) {
    fModule = llvm::make_unique<llvm::Module>(moduleName, fContext);
  }

  llvm::Module* getModule() {
    return fModule.get();
  }

  std::unique_ptr<llvm::Module> getModuleOwnership() {
    return std::move(fModule);
  }

  llvm::LLVMContext& getContext() {
    return fContext;
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
    llvm::Function* function = llvm::Function::Create(type, llvm::Function::ExternalLinkage, func.getName(), fModule.get());
    llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(fContext, "entry", function);
    fBuilder.SetInsertPoint(entryBlock);
    for (auto stmt: func.getBlockStmt()->list) {
      if (std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
        fBuilder.CreateRet(transformExpr(*std::dynamic_pointer_cast<ReturnStmt>(stmt)->expr));
      } else {
        throw std::logic_error("only return statements are currently supported");
      }
    }
    return function;
  }

  llvm::Value* transformExpr(const Expr& expr) {
    if (dynamic_cast<const IntegerExpr*>(&expr)) {
      return llvm::ConstantInt::get(transformType(*expr.getType()), (uint64_t)(dynamic_cast<const IntegerExpr&>(expr).getInt()), true);
    } else {
      throw std::logic_error("only integer expressions currently supported");
    }
  }
};

#endif
