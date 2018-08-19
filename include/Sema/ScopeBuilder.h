#ifndef SEMA_SCOPE_BUILDER_H
#define SEMA_SCOPE_BUILDER_H

class ScopeBuilder {
private:
  const class FuncDecl* function_;

public:
  /**
   * Recursively builds the lexical scope for the entire compilation
   * unit.
   *
   * 1. Creates unit scope
   * 2. Stores scope in CompilationUnit AST
   * 3. Iterates through all declarations in the unit
   * 4. Add declatation to UnitScope
   * 5. If function
   *    a. Creates Function Scope
   *    b. Stores function scope in FuncDecl AST
   *    c. Parents declatation scope to unit scope
   *    d. Calls buildFunctionScope.
   *
   * Note that because global declarations are processed linearly, functions
   * and global variables must be declared before they are used.
   */
  void buildCompilationUnitScope(class CompilationUnit&);

  /**
   * Recursively builds the lexical scope for the entire function.
   * The involves the following steps.
   *
   * 1. Add all ParamDecls to the function scope.
   * 2. Create Block scope.
   * 2. Parents block scope to function scope.
   * 3. Store Block scope in CompoundStmt ast.
   * 3. Calls buildBlockScope
   */
  void buildFunctionScope(class FuncDecl&);

  // does stuff
  void buildStmtScope(class Stmt& stmt, class DeclContext *parent);
  /**
   * Recursivly builds the lexical scope for a block stmt with the following
   * steps.
   *
   * 1. Iterate through statements
   *    a. If VarDecl or LetDecl
   *       - add to block scope
   *    b. If WhileLoop
   *       - create loop scope
   *       - add loop scope to WhileLoop AST
   *       - parent loop scope to block scope.
   *       - process with buildWhileLoopScope
   *    c. If ConditionalStmtBlock
   *       - iterate through conditionalStmts
   *       - create conditional stmt scope.
   *       - add conditional stmt scope to ConditionalStmt AST
   *       - parent conditional stmt scope to block scope
   *       - process with buildConditionalStmtScope
   */
  void buildCompoundStmtScope(class CompoundStmt&);


  /**
   * Recursivly builds the lexical scope for a while loop with the following
   * steps.
   *
   * 1. Create while loop scope
   * 2. Create block scope
   * 3. Parent block scope to loop scope.
   * 4. Process block scope with buildBlockScope.
   *
   * Note that the while loop scope does not currently contain any declarations.
   * however, eventually, the tree will be extended to allow LetDecl
   * as the condtition. The value of the LetDecl will be used as the condition
   * and the LetDecl will be added to the WhileLoopScope.
   */
  void buildWhileLoopScope(class WhileLoop&);

  /**
   * Recursivly builds the lexical scope for a ConditionalStmt loop with
   * the following steps.
   *
   * 1. Create ConditionalStmt scope
   * 2. Create block scope
   * 3. Parent block scope to ConditionalStmt scope.
   * 4. Process block scope with buildBlockScope.
   *
   * Note that the while ConditionalStmt does not currently contain any declarations.
   * however, eventually, the tree will be extended to allow LetDecl
   * as the condtition. The value of the LetDecl will be used as the condition
   * and the LetDecl will be added to the ConditionalStmt scope.
   */
  void buildConditionalStmtScope(class ConditionalStmt&);
};
#endif
