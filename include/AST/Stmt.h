#ifndef AST_STMT_H
#define AST_STMT_H

#include "Basic/SourceCode.h"

#include "AST/DeclContext.h"
#include "AST/TreeElement.h"

#include <assert.h>
#include <memory>
#include <algorithm>

class Stmt;
class Decl;
class Expr;
class LetDecl;
class ReturnStmt;

/// The base class for all statements in the program. A statement is a single
/// logical instruction. When running, the program will execute a single
/// statement at a time. Statements can and should have "side effects" that
/// effect the state of the program or alter the flow.
class Stmt : public TreeElement {
public:
  /// An enumeration of all possible types derived from Stmt
  enum class Kind {
    #define STMT(SELF, PARENT) SELF,
    #include "AST/Stmt.def"
    #undef STMT
  };

  /// virtual destructor prevents memory leaks
  virtual ~Stmt() = default;

  /// Return the printable name of the TreeElement, which in this case is
  /// statement. Derived types should override this method with their own
  /// prefix, followed by "-statement"
  virtual std::string name() const override {
    return "statement";
  };

  /// Return true if this statement will cause the current function to exit
  virtual bool returns() const = 0;

  /// Return the runtime type of the statement
  virtual Stmt::Kind getKind() const = 0;
};

/// A compound stmt is a single stmt which is composed of many statements.
/// In textual form, it appears as a list of statements wrapped between "{" "}".
/// A compound stmt has its own scope...
///
/// Question: Currently, a compound statement cannot appear on its own, but only
/// as a child element of a function declaration, if stmt, or while stmt, all of
/// which have their own scope. Does giving a compound stmt its own scope make
/// sense?
class CompoundStmt : public Stmt {
private:
  DeclContext context_;
  std::vector<std::unique_ptr<Stmt>> stmts_;
public:
  /// Construct a CompoundStmt with the given list of stmts.
  CompoundStmt(std::vector<std::unique_ptr<Stmt>> stmts)
  : stmts_{std::move(stmts)} {
    assert(std::find(stmts_.begin(), stmts_.end(), nullptr) == stmts_.end()
      && "precondition: stmts must not contain nullptr");
  }

  /// Return the declaration context owned by the compound stmt
  DeclContext* getDeclContext() {
    return &context_;
  }

  /// Return the printable name of the TreeElement
  std::string name() const override {
    return "compound-statement";
  };

  /// Sets the parent context of the DeclContext. This will allow children to
  /// access declarations in higher scopes.
  void setParentContext(DeclContext *parent) {
    context_.setParentContext(parent);
  }

  /// Return a const reference to the enclosed stmt list
  const std::vector<std::unique_ptr<Stmt>>& getStmts() const {
    return stmts_;
  };

  /// Return the runtime type, which is Stmt::Kind::CompoundStmt
  Stmt::Kind getKind() const override { return Kind::CompoundStmt;}

  /// Return true if this statement will cause the current function to exit.
  /// This is only true if any of the stmts in the list return.
  bool returns() const override {
    for (auto &stmt: stmts_) {
      if (stmt->returns()) return true;
    }
    return false;
  }

  /// Return the child tree elements for walking and serialization.
  std::vector<TreeElement*> getChildren() const override {
    std::vector<TreeElement*> treeVector;
    for (auto &stmt: stmts_) {
      treeVector.push_back(stmt.get());
    }
    return treeVector;
  }
};

/// Represents a statement that will only execute given if a given condition is
/// met at runtime. The condition can either be an expression, or a nullable
/// let declaration. If the condition is an expression, the stmt will execute if
/// the expression evaluates to true. If the condition is a let declaration, the
/// stmt will only execute if the let declaration is non null.
class ConditionalStmt : public Stmt {
private:
  DeclContext context;
  std::unique_ptr<LetDecl> declaration_;
  std::unique_ptr<Expr> condition_;
  std::unique_ptr<CompoundStmt> stmt_;

public:
  /// Construct a ConditionalStmt with the given condition and block stmt.
  ConditionalStmt(std::unique_ptr<Expr> condition, std::unique_ptr<CompoundStmt> stmt)
  : condition_{std::move(condition)}, stmt_{std::move(stmt)} {
    assert(condition_ && "precondition: condition is required");
    assert(stmt_ && "precondition: statement is required");
  }

  /// Construct a ConditionalStmt with the given declaration and block stmt.
  ConditionalStmt(std::unique_ptr<LetDecl> declaration, std::unique_ptr<CompoundStmt> stmt)
  : declaration_{std::move(declaration)}, stmt_{std::move(stmt)} {
    assert(declaration_ && "precondition: declaration is required");
    assert(stmt_ && "precondition: statement is required");
  }

  /// Return the declaration context owned by this statement. It will
  /// encapsulate the declaration condition... if it exists.
  DeclContext* getDeclContext() {
    return &context;
  }

  /// Return the printable name of the statement
  std::string name() const override {
    return "conditional-statement";
  };

  /// Sets the parent of the declaration context, to allow children to access
  /// higher level declarations.
  void setParentContext(DeclContext *parent) {
    context.setParentContext(parent);
  }

  /// Return the declaration... which may be nullptr
  LetDecl* getDeclaration() {
    return declaration_.get();
  }

  /// Return the condition... which may be nullptr
  Expr* getCondition() {
      return condition_.get();
  }

  /// Return a const reference to the block
  const CompoundStmt& getBlock() const {
    return *stmt_;
  }

  /// Return a reference to the block
  CompoundStmt& getBlock() {
    return *stmt_;
  }

  /// Return the runtime kind of the stmt.
  Stmt::Kind getKind() const override { return Kind::ConditionalStmt;}

  /// Return false because the stmt is not guarenteed to return
  bool returns() const override {
    return stmt_->returns();
  }

  /// Return the child nodes for walking or serialization
  std::vector<TreeElement*> getChildren() const override;
};

/// Represents a logic block of conditional stmts. Many conditional stmts can
/// be logically joined together into if, else if, else blocks. Thus, a
/// conditional block is composed of a list of statements... which can either
/// be a conditional stmt or a compound statement ( in the case of an else )
class ConditionalBlock : public Stmt {
private:
  std::vector<std::unique_ptr<Stmt>> stmts_;
public:

  /// Construct a ConditionalBlock with the given stmts
  ConditionalBlock(std::vector<std::unique_ptr<Stmt>> stmts)
  : stmts_{std::move(stmts)} {

    // assert( std::find_if_not(stmts_.begin(), stmts_.end(), [](auto &stmt) {
    //   return dynamic_cast<ConditionalStmt*>(stmt.get()) != nullptr
    //       || dynamic_cast<CompoundStmt*>(stmt.get()) != nullptr;
    //   }) == stmts_.end()
    // && "precondition: stmts must contain only ConditionalStmt or CompoundStmt");

  }

  /// Return the list of if, else if, and else stmts
  std::vector<std::unique_ptr<Stmt>>& getStmts() {
    return stmts_;
  }

  /// Return the runtime type
  Stmt::Kind getKind() const override {
    return Stmt::Kind::ConditionalBlock;
  }

  /// Return the printable name of the element
  std::string name() const override {
    return "conditional-block-statement";
  };

  /// Return the child elements for traversal and serialization
  std::vector<TreeElement*> getChildren() const override {
    std::vector<TreeElement*> children;
    for (auto &stmt: stmts_) {
      children.push_back(stmt.get());
    }
    return children;
  }

  /// Returns true if the statement is guarenteed to return
  virtual bool returns() const override {
    // if a conditional block does not contain an else statement... it can not
    // be guarenteed to return
    if (dynamic_cast<ConditionalStmt*>(stmts_.back().get())) return false;

    for (auto &stmt: stmts_) {
      if (!stmt->returns()) return false;
    }

    return true;
  }

};

/// Represents a loop that executes as long as the condition is true, and checks
/// the condition before each iteration. The condition can either be an
/// expression which must evaulate to tree, or a let declaration which must be
/// non null.
class WhileLoop : public Stmt {
private:
  DeclContext context_;
  std::unique_ptr<LetDecl> decl_;
  std::unique_ptr<Expr> condition_;
  std::unique_ptr<CompoundStmt> stmt_;

public:

  /// Constructs a while loop with the given declaration and block
  WhileLoop(std::unique_ptr<LetDecl> decl, std::unique_ptr<CompoundStmt> stmt)
  : decl_{std::move(decl)}, stmt_{std::move(stmt)} {
    assert(decl_ && "precondition: declaration must not be nullptr");
    assert(stmt_ && "precondition: statement is required");
  }
  /// Constructs a while loop with the given condtion and block
  WhileLoop(std::unique_ptr<Expr> condition, std::unique_ptr<CompoundStmt> stmt)
  : condition_{std::move(condition)}, stmt_{std::move(stmt)} {
    assert(condition_ && "precondition: condition must not be nullptr");
    assert(stmt_ && "precondition: stmt must not be nullptr");
  }

  /// A while loop is never guarenteed to execute... therefore it can never be
  /// guarenteed to return.
  bool returns() const override {
    return false;
  }

  /// Return the runtime type of the statement
  Stmt::Kind getKind() const override { return Kind::WhileLoop;}

  std::vector<TreeElement*> getChildren() const override;

  std::string name() const override {
    return "while-loop-statement";
  };

  LetDecl* getDeclaration() {
    return decl_.get();
  }

  Expr* getCondition() {
      return condition_.get();
  }

  CompoundStmt* getBlock() {
    return stmt_.get();
  }

  DeclContext* getDeclContext() {
    return &context_;
  }

  void setParentContext(DeclContext *parent) {
    context_.setParentContext(parent);
  }
};

/// Represents an Statement that when run, exits the current function with
/// the given expression. If the ReturnStmt contains a nullptr expression, then
/// the function returns with a void value.
class ReturnStmt : public Stmt{
private:
  std::unique_ptr<Expr> expr_;

public:
  /// Constructs a ReturnStmt with the given expr, which may be nullptr.
  ReturnStmt(std::unique_ptr<Expr> expr): expr_{std::move(expr)} {  }

  /// Return true because by definition, a return stmt will always return
  bool returns() const override {
    return true;
  }

  /// Return a const pointer to the return expression if it exists, otherwise
  /// nullptr
  const Expr* getExpr() const {
    return expr_.get();
  }

  /// Return the printable name of the stmt.
  std::string name() const override {
    return "return-statement";
  };

  /// Return a pointer to the return expression if it exists, otherwise nullptr
  Expr* getExpr() {
    return expr_.get();
  }

  /// Return the runtime type, which is Stmt::Kind::ReturnStmt
  Stmt::Kind getKind() const override { return Kind::ReturnStmt;}

  /// Return the child nodes for walking
  std::vector<TreeElement*> getChildren() const override;
};

/// Represents an Expression which appears in a list of stmts. This is required
/// so that Expressions with side effects may appear on their own in a block.
/// For example, a function call.
class ExprStmt : public Stmt {
private:
  std::unique_ptr<Expr> expr_;

public:
  /// Construct an ExprStmt with the given Expr
  ExprStmt(std::unique_ptr<Expr> e): expr_{std::move(e)} {
    assert(expr_ && "precondition: decl must not be nullptr");
  }

  /// Return false because an Expr will never return. It should have no direct
  /// effect on the control flow
  bool returns() const override {
    return false;
  }

  /// Return the runtime type, which is Stmt::Kind::ExprStmt
  Stmt::Kind getKind() const override { return Kind::ExprStmt;}

  /// Return the child nodes for walking
  std::vector<TreeElement*> getChildren() const override;

  /// Return the printable name of the stmt
  std::string name() const override {
    return "expression-statement";
  };

  /// Return a const pointer to the Expr.
  const Expr* getExpr() const {
    return expr_.get();
  }

  /// Return a pointer to the Expr.
  Expr* getExpr() {
    return expr_.get();
  }
};

/// Represents a Declaration which appears in a list of stmts. This is required
/// so that Decls and Stmts may be mixed in a single block. The Decl should
/// only be referenced by statements following in it the list of stmts.
class DeclStmt : public Stmt {
private:
  std::unique_ptr<Decl> decl_;

public:
  /// Construct a DeclStmt wiht the given Decl
  DeclStmt(std::unique_ptr<Decl> decl): decl_{std::move(decl)} {
    assert(decl_ && "precondition: decl must not be nullptr");
  }

  /// Return the runtime type of the Stmt, which is Stmt::Kind::DeclStmt
  Stmt::Kind getKind() const override { return Kind::DeclStmt;}

  /// Return the child nodes for walking
  std::vector<TreeElement*> getChildren() const override;

  /// Return false because a DeclStmt will never return. It is a logical
  /// statement with no side effects.
  bool returns() const override {
    return false;
  }

  /// Return the printable name of the stmt
  std::string name() const override {
    return "declaration-statement";
  };

  /// Return a const pointer to the Decl.
  const Decl* getDecl() const {
    return decl_.get();
  }

  /// Return a pointer to the Decl.
  Decl* getDecl() {
    return decl_.get();
  }
};

/// Represents a single file. Each CompilationUnit has its own unique
/// Declaration context, which contains file-scoped declarations such as
/// functions. It also contains a list of file-scoped stmts. Currently, the
/// only allowed stmt is a DeclStmt whose decl is a FuncDecl.
class CompilationUnit : public Stmt {
private:
  std::vector<std::unique_ptr<Stmt>> stmts_;
  DeclContext context_;
public:
  /// Construct a CompilationUnit with the given vector of stmts. The vector
  /// is a vector of unique_ptr so that the unit is guarenteed to own it's
  /// statements.
  CompilationUnit(std::vector<std::unique_ptr<Stmt>> stmts)
  : stmts_{std::move(stmts)} {
    assert(std::find(stmts_.begin(), stmts_.end(), nullptr) == stmts_.end()
      && "precondition: stmts must not contain nullptr");
  };

  /// Return the runtime type of the Stmt, which is Stmt::Kind::CompilationUnit
  Stmt::Kind getKind() const override {
    return Kind::CompilationUnit;
  }

  /// Return the most specific declaration context available to the unit. A unit
  /// has its own file-scope, so this method will return its own context.
  DeclContext* getDeclContext() {
    return &context_;
  }

  std::string name() const override {
    return "compilation-unit";
  };

  /// Return a vector of child elements for walking.
  std::vector<TreeElement*> getChildren() const override;

  /// Return true if the stmt is guarenteed to return. This is meaningless on a
  /// unit stmt, so always return true
  bool returns() const override {
    return true;
  }

  /// Return a const reference to the stmt vector
  const std::vector<std::unique_ptr<Stmt>>& stmts() const {
    return stmts_;
  }

  /// Return a reference to the stmt vector
  std::vector<std::unique_ptr<Stmt>>& stmts() {
    return stmts_;
  }
};

#endif
