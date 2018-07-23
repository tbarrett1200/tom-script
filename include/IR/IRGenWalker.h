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
#include <vector>
#include <map>

class LLVMTransformer {
private:
  llvm::LLVMContext& fContext;
  llvm::Module* fModule;
  llvm::BasicBlock *fCurrentBlock;

  std::map<std::string, llvm::Value*> fNamedValues;
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
    } else if (type.isDoubleType()) {
      return llvm::Type::getDoubleTy(fContext);
    } else {
      throw std::logic_error("only integer and double types are currently supported");
    }
  }

  llvm::Function* transformFunction(const FuncDecl &func) {
    llvm::FunctionType* type = transformFunctionType(*std::dynamic_pointer_cast<FunctionType>(func.getType()));
    llvm::Function* function = llvm::Function::Create(type, llvm::Function::ExternalLinkage, func.getName(), fModule);

    int index = 0;
    for (auto &arg : function->args()) {
      ParamDecl *param = func.getParams()[index++].get();
      arg.setName(param->getName());
      fNamedValues[param->getName()] = &arg;
    }

    fCurrentBlock = llvm::BasicBlock::Create(fContext, "entry", function);
    llvm::IRBuilder<> builder{fCurrentBlock};
    for (auto stmt: func.getBlockStmt()->list) {
      if (std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
        builder.CreateRet(transformExpr(*std::dynamic_pointer_cast<ReturnStmt>(stmt)->expr));
      } else if (std::dynamic_pointer_cast<DeclStmt>(stmt)) {
        const DeclStmt* declStmt = dynamic_cast<const DeclStmt*>(stmt.get());
        const Decl* decl = dynamic_cast<const Decl*>(declStmt->decl.get());
        if (dynamic_cast<const LetDecl*>(decl)) {
          const LetDecl *letDecl = dynamic_cast<const LetDecl*>(decl);
          llvm::Value *v = transformExpr(*letDecl->expr);
          fNamedValues[letDecl->getName()] = v;
        } else {
          throw std::logic_error("only let declarations are currently supported");
        }
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
    } else if (dynamic_cast<const IdentifierExpr*>(&expr)) {
      const IdentifierExpr& identifierExpr = dynamic_cast<const IdentifierExpr&>(expr);
      return fNamedValues[identifierExpr.getLexeme()];
    } else {
      throw std::logic_error("only integer and double expressions currently supported ");
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
