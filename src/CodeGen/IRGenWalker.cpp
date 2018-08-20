#include "CodeGen/IRGenWalker.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CFG.h"
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

llvm::FunctionType* LLVMTransformer::transformFunctionType(const FunctionType &type) {
  if (type.getParamTypes().size() == 0) {
    return llvm::FunctionType::get(transformType(*type.getReturnType()), false);
  } else {
    std::vector<llvm::Type*> paramTypes;
    for (auto paramType: type.getParamTypes()) {
      paramTypes.push_back(transformType(*paramType));
    }
    return llvm::FunctionType::get(transformType(*type.getReturnType()), paramTypes, false);
  }
}

llvm::Type* LLVMTransformer::transformType(const Type &type) {
  if (type.isIntegerType()) {
    return llvm::Type::getInt64Ty(context_);
  } if (type.isBooleanType()) {
    return llvm::Type::getInt1Ty(context_);
  } else if (type.isDoubleType()) {
    return llvm::Type::getDoubleTy(context_);
  } else if (type.getKind() == Type::Kind::ListType) {
    const ListType &list_type = dynamic_cast<const ListType&>(type);
    return llvm::ArrayType::get(transformType(*list_type.element_type()), list_type.size());
  } else if (type.getKind() == Type::Kind::CharacterType) {
    return llvm::Type::getInt8Ty(context_);
  } else if (type.getKind() == Type::Kind::PointerType) {
    const PointerType &ptr_type = dynamic_cast<const PointerType&>(type);
    return llvm::PointerType::getUnqual(transformType(*ptr_type.getReferencedType()));
  } else if (type.getKind() == Type::Kind::ReferenceType) {
    const ReferenceType &ptr_type = dynamic_cast<const ReferenceType&>(type);
    return llvm::PointerType::getUnqual(transformType(*ptr_type.getReferencedType()));
  } else {
    throw std::logic_error(type.toString() + " not supported by codegen");
  }
}

void LLVMTransformer::transformReturnStmt(const ReturnStmt& stmt, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};
  if (const Expr *expr = stmt.getExpr()) {
    builder.CreateRet(transformExpr(*expr, current_block));
  } else {
    builder.CreateRetVoid();
  }
}

void LLVMTransformer::transformLetDecl(const LetDecl& let_decl, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};
  llvm::AllocaInst *alloca = builder.CreateAlloca(transformType(*let_decl.getType()), 0, let_decl.getName().str());
  builder.CreateStore(transformExpr(let_decl.getExpr(),current_block), alloca);
  named_values_[let_decl.getName()] = alloca;
}

void LLVMTransformer::transformVarDecl(const VarDecl& var_decl, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};
  llvm::AllocaInst *alloca = builder.CreateAlloca(transformType(*var_decl.getType()), 0, var_decl.getName().str());
  if (var_decl.getType()->getKind() == Type::Kind::ReferenceType) {
    builder.CreateStore(transformLeftValueExpr(var_decl.getExpr(),current_block), alloca);
  } else {
    builder.CreateStore(transformExpr(var_decl.getExpr(),current_block), alloca);
  }
  named_values_[var_decl.getName()] = alloca;
}

void LLVMTransformer::transformUninitializedVarDecl(const UninitializedVarDecl& var_decl, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};
  llvm::AllocaInst *alloca = builder.CreateAlloca(transformType(*var_decl.getType()), 0, var_decl.getName().str());
  named_values_[var_decl.getName()] = alloca;
}

void LLVMTransformer::transformDeclStmt(const DeclStmt& declStmt, llvm::BasicBlock* current_block) {
  const Decl* decl = dynamic_cast<const Decl*>(declStmt.getDecl());
  switch (decl->getKind()) {
    case Decl::Kind::LetDecl:
      transformLetDecl(dynamic_cast<const LetDecl&>(*decl), current_block);
      break;
    case Decl::Kind::VarDecl:
      transformVarDecl(dynamic_cast<const VarDecl&>(*decl), current_block);
      break;
    case Decl::Kind::UninitializedVarDecl:
      transformUninitializedVarDecl(dynamic_cast<const UninitializedVarDecl&>(*decl), current_block);
      break;
    case Decl::Kind::FuncDecl:
      throw CompilerException(nullptr, "nested function declarations not current supported");
      break;
    default:
      throw CompilerException(nullptr, "unable to generate code for this decl type");
  }
}


llvm::Value* LLVMTransformer::transformLeftValueIdentifierExpr(const IdentifierExpr& id_expr, llvm::BasicBlock* current_block) {
  auto map_it = named_values_.find(id_expr.lexeme());
  if (map_it != named_values_.end()) {
    return map_it->second;
  } else {
    std::stringstream ss;
    ss << "codegen: something went wrong: unable to find " << id_expr.name();
    throw CompilerException(nullptr, ss.str());
  }
}

llvm::Value* LLVMTransformer::transformLeftValueAccessorExpr(const AccessorExpr &accessor, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};

  const Type* aggregate_type = accessor.identifier().getType();

  if (const ListType *list_type = dynamic_cast<const ListType*>(aggregate_type)) {
    const Type* element_type = list_type->element_type();

    llvm::Value* aggregate_loc = transformLeftValueExpr(
      accessor.identifier()
    , current_block
    );

    llvm::Type* index_type = transformType(*IntegerType::getInstance());
    llvm::Value* element_index_0 = llvm::ConstantInt::get(index_type, 0, true);
    llvm::Value* element_index_1 = llvm::ConstantInt::get(index_type, accessor.index(), true);
    std::array<llvm::Value*,2> indices{{element_index_0, element_index_1}};
    return builder.CreateGEP(aggregate_loc, indices);

  } else if (const ReferenceType *ref_type = dynamic_cast<const ReferenceType*>(aggregate_type)) {
    // guard to make sure reference is of proper type
    if (ref_type->getReferencedType()->getKind() != Type::Kind::ListType) {
      std::stringstream ss;
      ss << "codegen: unchecked: unable to access element of " << ref_type->toString();
      throw CompilerException(nullptr, ss.str());
    }

    const ListType *list_type = dynamic_cast<const ListType*>(ref_type->getReferencedType());
    const Type* element_type = list_type->element_type();

    // access the pointer value
    llvm::Value* aggregate_loc = transformExpr(
      accessor.identifier()
    , current_block
    );

    llvm::Type* index_type = transformType(*IntegerType::getInstance());
    llvm::Value* element_index_0 = llvm::ConstantInt::get(index_type, 0, true);
    llvm::Value* element_index_1 = llvm::ConstantInt::get(index_type, accessor.index(), true);
    std::array<llvm::Value*,2> indices = {{element_index_0, element_index_1}};
    return builder.CreateGEP(aggregate_loc, indices);

  } else {
    throw CompilerException(nullptr, "codegen: unchecked: illegal accessor expr");
  }
}

llvm::Value* LLVMTransformer::transformLeftValueExpr(const Expr& expr, llvm::BasicBlock* current_block) {

  if (expr.getType()->getKind() == Type::Kind::ReferenceType) {
    return transformExpr(expr, current_block);
  }

  if (expr.isLeftValue()) {
    switch (expr.getKind()) {
      case Expr::Kind::IdentifierExpr:
        return transformLeftValueIdentifierExpr(dynamic_cast<const IdentifierExpr&>(expr), current_block);
      case Expr::Kind::AccessorExpr:
        return transformLeftValueAccessorExpr(dynamic_cast<const AccessorExpr&>(expr), current_block);
      default:
        std::stringstream ss;
        ss << "codegen: unimplemented: unable to reference '" << expr.name() << "'";
        throw CompilerException(nullptr, ss.str());
    }
  } else {
    throw CompilerException(nullptr, "unable to reference r-value expression");
  }
}

llvm::Value* LLVMTransformer::transformAssignmentStmt(const BinaryExpr& bin_expr, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};
  if (bin_expr.getLeft().getType()->getKind() == Type::Kind::ReferenceType) {
    llvm::Value *lval = transformLeftValueExpr(bin_expr.getLeft(), current_block);
    llvm::Value *rval = transformLeftValueExpr(bin_expr.getRight(), current_block);
    builder.CreateStore(rval, lval);
    return rval;
  } else if (bin_expr.getLeft().isLeftValue()) {
    llvm::Value *lval = transformLeftValueExpr(bin_expr.getLeft(), current_block);
    llvm::Value *rval = transformExpr(bin_expr.getRight(), current_block);
    builder.CreateStore(rval, lval);
    return rval;
  } else {
    std::stringstream ss;
    ss << "unable to assign to r-value type";
    throw CompilerException(nullptr, ss.str());
  }
}

llvm::Function* LLVMTransformer::transformExternalFunctionDecl(const ExternFuncDecl &extern_func) {
  llvm::FunctionType* type = transformFunctionType(*extern_func.getFunctionType());
  return llvm::Function::Create(type, llvm::Function::ExternalLinkage, extern_func.getName().str(), module_);
}

llvm::Function* LLVMTransformer::transformFunction(const FuncDecl &func) {
  currentContext = func.getDeclContext();

  llvm::FunctionType* type = transformFunctionType(dynamic_cast<const FunctionType&>(*func.getType()));
  function_ = llvm::Function::Create(type, llvm::Function::ExternalLinkage, func.getName().str(), module_);

  int index = 0;
  for (auto &arg : function_->args()) {
    ParamDecl *param = func.getParams()[index++].get();
    arg.setName(param->getName().str());
    named_values_[param->getName()] = &arg;
  }

  llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(context_, "entry", function_);

  transformCompoundStmt(func.getBlockStmt(), entry_block);

  return function_;
}

llvm::Value* LLVMTransformer::transformListExpr(const ListExpr& list, llvm::BasicBlock* current_block) {
  const ListType* list_type = dynamic_cast<const ListType*>(list.getType());
  llvm::ArrayType *array_type =  llvm::ArrayType::get(transformType(*list_type->element_type()), list.elements().size());
  std::vector<llvm::Constant*> elements;

  if (list_type->element_type()->isIntegerType()) {
    llvm::Type* type = transformType(*IntegerType::getInstance());
    for (auto &element: list.elements()) {
      elements.push_back(llvm::ConstantInt::get(type, (uint64_t)(dynamic_cast<const IntegerExpr&>(*element).getInt()), true));
    }
  } else if (list_type->element_type()->isDoubleType()) {
    llvm::Type* type = transformType(*DoubleType::getInstance());
    for (auto &element: list.elements()) {
        elements.push_back(llvm::ConstantFP::get(type, (dynamic_cast<const DoubleExpr&>(*element).getDouble())));
    }
  } else if (list_type->element_type()->getKind() == Type::Kind::CharacterType) {
    llvm::Type* type = transformType(*CharacterType::getInstance());
    for (auto &element: list.elements()) {
        elements.push_back(llvm::ConstantInt::get(type, (dynamic_cast<const CharacterExpr&>(*element).getChar())));
    }
  } else {
    throw CompilerException(nullptr, "array initializer only allowed for literals");
  }

  return llvm::ConstantArray::get(array_type, elements);
}

llvm::Value* LLVMTransformer::transformFunctionCall(const FunctionCall& call, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};

  if (call.getFunctionName() == StringRef{"Double"}) {
    llvm::Value *arg1 = transformExpr(*call.getArguments()[0], current_block);
    llvm::Type *t = transformType(*DoubleType::getInstance());
    return builder.CreateSIToFP(arg1, t);
  } else if (call.getFunctionName() == StringRef{"Int"}) {
    llvm::Value *arg1 = transformExpr(*call.getArguments()[0], current_block);
    llvm::Type *t = transformType(*IntegerType::getInstance());
    return builder.CreateFPToSI(arg1, t);
  }

  //  return named_values_[identifierExpr.getLexeme()];
  llvm::Function *CalleeF = module_->getFunction(call.getFunctionName().str());
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

llvm::Value* LLVMTransformer::transformIdentifierExpr(const IdentifierExpr& expr, llvm::BasicBlock* current_block) {
  auto map_it = named_values_.find(expr.lexeme());
  if (map_it != named_values_.end()) {
    llvm::IRBuilder<> builder{current_block};
    return builder.CreateLoad(map_it->second);
  } else {
    std::stringstream ss;
    ss << "unable to access '" << expr.lexeme() << "' during codegen";
    throw CompilerException(nullptr, ss.str());
  }
}

llvm::Value* LLVMTransformer::transformExpr(const Expr& expr, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};

  if (dynamic_cast<const IntegerExpr*>(&expr)) {
    return llvm::ConstantInt::get(transformType(*expr.getType()), (uint64_t)(dynamic_cast<const IntegerExpr&>(expr).getInt()), true);
  } else if (dynamic_cast<const DoubleExpr*>(&expr)) {
    return llvm::ConstantFP::get(transformType(*expr.getType()), (dynamic_cast<const DoubleExpr&>(expr).getDouble()));
  } else if (dynamic_cast<const CharacterExpr*>(&expr)) {
    return llvm::ConstantInt::get(transformType(*expr.getType()), (dynamic_cast<const CharacterExpr&>(expr).getChar()));
  } else if (dynamic_cast<const BoolExpr*>(&expr)) {
    return llvm::ConstantInt::get(transformType(*expr.getType()), dynamic_cast<const BoolExpr&>(expr).getBool()?1:0);
  } else if (dynamic_cast<const BinaryExpr*>(&expr)) {
    return transformBinaryExpr(dynamic_cast<const BinaryExpr&>(expr),current_block);
  } else if (dynamic_cast<const UnaryExpr*>(&expr)) {
    return transformUnaryExpr(dynamic_cast<const UnaryExpr&>(expr),current_block);
  } else if (dynamic_cast<const IdentifierExpr*>(&expr)) {
    return transformIdentifierExpr(dynamic_cast<const IdentifierExpr&>(expr),current_block);
  } else if (dynamic_cast<const FunctionCall*>(&expr)) {
    return transformFunctionCall(dynamic_cast<const FunctionCall&>(expr),current_block);
  } else if (dynamic_cast<const ListExpr*>(&expr)) {
    return transformListExpr(dynamic_cast<const ListExpr&>(expr),current_block);
  } else if (const StringExpr *string_expr = dynamic_cast<const StringExpr*>(&expr)) {
    return llvm::ConstantDataArray::getString(context_, string_expr->getString());
  } else if (const AccessorExpr *accessor_expr = dynamic_cast<const AccessorExpr*>(&expr)) {
    return builder.CreateLoad(transformLeftValueAccessorExpr(*accessor_expr, current_block));
  } else {
    std::stringstream ss;
    ss << "unimplemented: unable to transform " << expr.name();
    throw CompilerException(nullptr, ss.str());
  }
}

llvm::BasicBlock* LLVMTransformer::transformStmt(Stmt& stmt, llvm::BasicBlock *current_block) {
  if (ReturnStmt* ret_stmt = dynamic_cast<ReturnStmt*>(&stmt)) {
    transformReturnStmt(*ret_stmt, current_block);
    return current_block;
  } else if (DeclStmt* decl_stmt = dynamic_cast<DeclStmt*>(&stmt)) {
    transformDeclStmt(*decl_stmt, current_block);
    return current_block;
  } else if (ExprStmt *expr_stmt = dynamic_cast<ExprStmt*>(&stmt)) {
    transformExpr(*expr_stmt->getExpr(), current_block);
    return current_block;
  } else if (ConditionalBlock* cond_block = dynamic_cast<ConditionalBlock*>(&stmt)) {
    return transformConditionalBlock(*cond_block, current_block);
  } else if (WhileLoop *while_loop = dynamic_cast<WhileLoop*>(&stmt)) {
    return transformWhileLoop(*while_loop, current_block);
  } else if (CompoundStmt *comp_stmt = dynamic_cast<CompoundStmt*>(&stmt)) {
    return transformCompoundStmt(*comp_stmt, current_block);
  } else {
    throw std::logic_error("unsupported statement type");
  }
}

llvm::BasicBlock* LLVMTransformer::transformCompoundStmt(CompoundStmt& tree, llvm::BasicBlock *current_block) {
  for (auto it = tree.getStmts().begin(); it != tree.getStmts().end(); it++) {
    Stmt* stmt = it->get();
    current_block = transformStmt(*stmt, current_block);
  }
  return current_block;
}



llvm::BasicBlock* LLVMTransformer::transformConditionalBlock(
  ConditionalBlock& tree,
  llvm::BasicBlock* current_block
) {

  llvm::BasicBlock *if_exit = llvm::BasicBlock::Create(context_, "if_exit", function_);

  // for simplicity of debugging, create seperate cond_case block
  llvm::IRBuilder<> entry_builder{current_block};
  llvm::BasicBlock *if_cond = llvm::BasicBlock::Create(context_, "if_cond", function_);
  entry_builder.CreateBr(if_cond);

  for (auto it = tree.getStmts().begin(); it != tree.getStmts().end(); it++) {
    Stmt* stmt = it->get();
    bool last_block = std::distance(it, tree.getStmts().end()) == 1;


    // the alternative block must be generated in all cases except 'else'
    llvm::BasicBlock *next_block = last_block ? nullptr : llvm::BasicBlock::Create(context_, "else_if_cond", function_);

    if (ConditionalStmt* cond_stmt = dynamic_cast<ConditionalStmt*>(stmt)) {
      transformConditionalStmt(*cond_stmt, if_cond, next_block, if_exit);
      if_cond = next_block;
    } else {
      if_cond->setName("else");
      llvm::BasicBlock *else_exit = transformStmt(*stmt, if_cond);
      if (!else_exit->getTerminator()) {
        llvm::IRBuilder<> else_exit_builder{else_exit};
        else_exit_builder.CreateBr(if_exit);
      }
    }
  }

  if (llvm::pred_begin(if_exit) ==  llvm::pred_end(if_exit)) {
    if_exit->removeFromParent();
  }
  return if_exit;
}

llvm::BasicBlock* LLVMTransformer::transformWhileLoop(
  WhileLoop& tree
, llvm::BasicBlock* entry_block
) {
  llvm::IRBuilder<> entry_builder{entry_block};

  // creates a block to compute the loop_condition
  llvm::BasicBlock *loop_cond = llvm::BasicBlock::Create(context_, "loop_cond", function_);
  // unconditionally break from the previous block to the entry block
  entry_builder.CreateBr(loop_cond);

  llvm::IRBuilder<> cond_builder{loop_cond};

  // computes the loop condition in the loop_cond
  llvm::Value* condition = transformExpr(*tree.getCondition(), loop_cond);

  // creates the loop_body_entry and the loop_exit block
  llvm::BasicBlock *loop_body_entry = llvm::BasicBlock::Create(context_, "loop_body_entry", function_);
  llvm::BasicBlock *loop_exit = llvm::BasicBlock::Create(context_, "loop_exit", function_);

  // conditonal break to either the loop_body_entry or the loop_exit depending
  // on the condition
  cond_builder.CreateCondBr(condition, loop_body_entry, loop_exit);

  // computes the loop_body starting from loop_body_entry and returning the exit
  llvm::BasicBlock* loop_body_exit = transformCompoundStmt(*tree.getBlock(), loop_body_entry);

  // creates a jump to the condition at the end of the loop body if a
  // terminator does not already exist
  if (!loop_body_exit->getTerminator()) {
    llvm::IRBuilder<> loop_body_exit_builder{loop_body_exit};
    loop_body_exit_builder.CreateBr(loop_cond);
  }

  // returns the exit point for the loop
  return loop_exit;
}


void LLVMTransformer::transformConditionalStmt(
  ConditionalStmt& tree
, llvm::BasicBlock* if_cond
, llvm::BasicBlock* next_block
, llvm::BasicBlock* if_exit
) {
  llvm::IRBuilder<> cond_builder{if_cond};

  // computes the condition in the given if_cond block
  llvm::Value* condition = transformExpr(*tree.getCondition(), if_cond);

  // creates the entry point for the if_body
  llvm::BasicBlock *if_body_entry = llvm::BasicBlock::Create(context_, "if_body_entry", function_);

  // creates a conditional break to the if_body_entry, if the condition
  // is true, and either the next_block, or the exit_block, depending on
  // their existence, if the condition is false
  if (next_block) cond_builder.CreateCondBr(condition, if_body_entry, next_block);
  else cond_builder.CreateCondBr(condition, if_body_entry, if_exit);

  // generates the body of the conditional stmt, entering at if_body_entry and
  // returns a handle to the body exit.
  llvm::BasicBlock *if_body_exit = transformCompoundStmt(tree.getBlock(), if_body_entry);

  // if the conditional body does not return, forward it to if_exit
  if (!if_body_exit->getTerminator()) {
    llvm::IRBuilder<> if_body_exit_builder{if_body_exit};
    if_body_exit_builder.CreateBr(if_exit);
  }
}

/// Adds the computation of a binary expression to the given BasicBlock and
/// returns a handle to its result as a llvm::Value*. This currently only
/// works for builtin operations and assignment. If a builtin operator call
/// which passed the semantic checking phase but is not yet implented is
/// called, then a Compiler exception is thrown with a "not implemented"
/// exception is thrown.
llvm::Value* LLVMTransformer::transformBinaryExpr(const BinaryExpr& expr, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};

  // assignment is special
  if (expr.getOperator() == StringRef{"="}) {
    return transformAssignmentStmt(expr, current_block);
  }

  llvm::Value *lval = transformExpr(expr.getLeft(), current_block);
  llvm::Value *rval = transformExpr(expr.getRight(), current_block);

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
    }
  } else if (expr.getType()->isBooleanType()) {
    if (expr.getOperator() == "&&") {
      return builder.CreateAnd(lval, rval);
    } else if (expr.getOperator() == "||") {
      return builder.CreateOr(lval, rval);
    }
  }

  std::stringstream ss;
  ss << "not implemented: binary operator '" << expr.getOperator() << "' of this type";
  throw CompilerException(nullptr, ss.str());
}


llvm::Value* LLVMTransformer::transformUnaryExpr(const UnaryExpr& expr, llvm::BasicBlock* current_block) {
  llvm::IRBuilder<> builder{current_block};

  if (expr.getOperator() == StringRef{"&"}) {
    return transformLeftValueExpr(expr.getExpr(), current_block);
  } else if (expr.getOperator() == StringRef{"*"}) {
    return transformExpr(expr.getExpr(), current_block);
  }

  llvm::Value *val = transformExpr(expr.getExpr(), current_block);

  if (val->getType()->isIntegerTy() && val->getType()->isIntegerTy()) {
    if (expr.getOperator() == "+") {
      return val;
    } else if (expr.getOperator() == "-") {
      return builder.CreateNeg(val);
    }
  } else if (val->getType()->isDoubleTy() && val->getType()->isDoubleTy()) {
    if (expr.getOperator() == "+") {
      return val;
    } else if (expr.getOperator() == "-") {
      return builder.CreateFNeg(val);
    }
  } else if (expr.getType()->isBooleanType()) {
    if (expr.getOperator() == "!") {
      return builder.CreateNot(val);
    }
  }

  std::stringstream ss;
  ss << "not implemented: binary operator '" << expr.getOperator() << "' of this type";
  throw CompilerException(nullptr, ss.str());
}
