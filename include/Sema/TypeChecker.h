#ifndef SEMA_TYPE_CHECKER
#define SEMA_TYPE_CHECKER

/**
 * The type checker uses the ASTScope to annotate every type with an
 * an expression. It uses a bottom up approach, which is necessary for
 * overloading. e.g. you dont know the type of a function call until the types
 * of its parameters are known.
 */
class TypeChecker {
private:
  class DeclContext* currentContext;

public:

  TypeChecker(DeclContext* current): currentContext{current} {};
  /**
   * This method annotates and checks all types in a compilation unit.
   * If the tree is fully type annotated and declarations have been fully
   * linked, then this method returns true. Otherwise it returns false.
   */
  void checkExpr(class Expr &expr);
  void checkCharacterExpr(class CharacterExpr &expr);
  void checkStringExpr(class StringExpr &expr);
  void checkIntegerExpr(class IntegerExpr &expr);
  void checkDoubleExpr(class DoubleExpr &expr);
  void checkListExpr(class ListExpr &expr);
  void checkIdentifierExpr(class IdentifierExpr &expr);
  void checkAssignmentExpr(class BinaryExpr &expr);
  void checkReferenceExpr(class UnaryExpr &expr);
  void checkDereferenceExpr(class UnaryExpr &expr);
  void checkTupleExpr(class TupleExpr &expr);
  void checkAccessorExpr(class AccessorExpr &expr);
  void checkBoolExpr(class BoolExpr &expr);
  void checkUnaryExpr(class UnaryExpr &expr);
  void checkBinaryExpr(class BinaryExpr &expr);
  void checkFunctionCall(class FunctionCall &expr);

};

#endif
