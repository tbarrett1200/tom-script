#ifndef AST_VISITOR_H
#define AST_VISITOR_H

template< class T> class ASTVisitorResult {
private:
  T fResult;
protected:
  void setResult(T aResult) {
    fResult = aResult;
  }
public:
  T getResult() {
    return fResult;
  }
};

class ExprVisitor {
public:
  virtual void visit(const class Expr&) = 0;
  virtual void visit(const class LabeledExpr&) = 0;
  virtual void visit(const class StringExpr &) = 0;
  virtual void visit(const class IntegerExpr &) = 0;
  virtual void visit(const class DoubleExpr &) = 0;
  virtual void visit(const class ListExpr&) = 0;
  virtual void visit(const class IdentifierExpr &) = 0;
  virtual void visit(const class TupleExpr &) = 0;
  virtual void visit(const class AccessorExpr &) = 0;
  virtual void visit(const class OperatorExpr&) = 0;
  virtual void visit(const class BoolExpr &) = 0;
  virtual void visit(const class UnaryExpr &) = 0;
  virtual void visit(const class BinaryExpr &) = 0;
  virtual void visit(const class FunctionCall&) = 0;
  virtual void visit(const class StackPointer &) = 0;
};


class ASTVisitor: public ExprVisitor {

};

#endif
