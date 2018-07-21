#ifndef IR_TRANSFORMER_H
#define IR_TRANSFORMER_H

#include "AST/ASTVisitor.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"


class IRTransformer: public ASTVisitor, public ASTVisitorResult<llvm::Value*> {
private:
  llvm::IRBuilder<>* TheBuilder;
  llvm::LLVMContext *TheContext;
public:

  void setContext(llvm::LLVMContext *aContext) {
    TheContext = aContext;
  }

  void setBuilder(llvm::IRBuilder<> *aBuilder) {
    TheBuilder = aBuilder;
  }

  void visit(const class Expr&) override;
  void visit(const class LabeledExpr&) override;
  void visit(const class StringExpr &) override;
  void visit(const class IntegerExpr &) override;
  void visit(const class DoubleExpr &) override;
  void visit(const class ListExpr&) override;
  void visit(const class IdentifierExpr &) override;
  void visit(const class TupleExpr &) override;
  void visit(const class AccessorExpr &) override;
  void visit(const class OperatorExpr&) override;
  void visit(const class BoolExpr &) override;
  void visit(const class UnaryExpr &) override;
  void visit(const class BinaryExpr &) override;
  void visit(const class FunctionCall&) override;
  void visit(const class StackPointer &) override;
};

#endif
