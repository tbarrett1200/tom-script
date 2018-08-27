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

class LLVMTransformer {
private:
  llvm::LLVMContext& context_;
  llvm::Module* module_;
  llvm::Function* function_;
  const DeclContext* currentContext;

  std::map<StringRef, llvm::Value*> named_values_;

public:
  LLVMTransformer(llvm::LLVMContext& context, llvm::Module* module) : context_{context} {
    module_ = module;
  }

  llvm::FunctionType* transformFunctionType(const FunctionType &type);

  llvm::StructType* transformStructType(const TupleType &type);

  llvm::Type* transformType(const Type &type);

  void transformReturnStmt(const ReturnStmt& stmt, llvm::BasicBlock* current_block);

  void transformLetDecl(const LetDecl& let_decl, llvm::BasicBlock* current_block);

  void transformVarDecl(const VarDecl& var_decl, llvm::BasicBlock* current_block);


  void transformUninitializedVarDecl(const UninitializedVarDecl& var_decl, llvm::BasicBlock* current_block);

  void transformDeclStmt(const DeclStmt& declStmt, llvm::BasicBlock* current_block);

  void transformStructDecl(const StructDecl& struct_decl);

  llvm::Value* transformIdentifierExprReference(const IdentifierExpr& id_expr, llvm::BasicBlock* current_block);

  llvm::Value* transformAccessorExprReference(const AccessorExpr &accessor, llvm::BasicBlock* current_block);

  llvm::Value* transformExprReference(const Expr& expr, llvm::BasicBlock* current_block);

  llvm::Value* transformAssignmentStmt(const BinaryExpr& bin_expr, llvm::BasicBlock* current_block);

  llvm::Constant* transformConstant(const Expr& expr);

  llvm::Constant* transformConstantListExpr(const ListExpr& list);

  llvm::Constant* transformConstantTupleExpr(const TupleExpr& list);

  llvm::Function* transformExternalFunctionDecl(const ExternFuncDecl &extern_func);

  llvm::Function* transformFunction(const FuncDecl &func);

  llvm::Value* transformFunctionCall(const FunctionCall& call, llvm::BasicBlock* current_block);

  llvm::Value* transformIdentifierExpr(const IdentifierExpr& expr, llvm::BasicBlock* current_block);

  llvm::Value* transformExpr(const Expr& expr, llvm::BasicBlock* current_block);

  llvm::BasicBlock* transformStmt(Stmt& stmt, llvm::BasicBlock *current_block);

  llvm::BasicBlock* transformCompoundStmt(CompoundStmt& tree, llvm::BasicBlock *current_block);

  llvm::BasicBlock* transformConditionalBlock(
    ConditionalBlock& tree,
    llvm::BasicBlock* current_block
  );

  llvm::BasicBlock* transformWhileLoop(
    WhileLoop& tree
  , llvm::BasicBlock* entry_block
  );


  void transformConditionalStmt(
    ConditionalStmt& tree
  , llvm::BasicBlock* if_cond
  , llvm::BasicBlock* next_block
  , llvm::BasicBlock* if_exit
);

  /// Adds the computation of a binary expression to the given BasicBlock and
  /// returns a handle to its result as a llvm::Value*. This currently only
  /// works for builtin operations and assignment. If a builtin operator call
  /// which passed the semantic checking phase but is not yet implented is
  /// called, then a Compiler exception is thrown with a "not implemented"
  /// exception is thrown.
  llvm::Value* transformBinaryExpr(const BinaryExpr& expr, llvm::BasicBlock* current_block);


  llvm::Value* transformUnaryExpr(const UnaryExpr& expr, llvm::BasicBlock* current_block);
};

#endif
