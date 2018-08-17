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

class Stmt : public TreeElement {
public:
  enum class Kind {
    #define STMT(SELF, PARENT) SELF,
    #include "AST/Stmt.def"
    #undef STMT
  };

  virtual ~Stmt() = default;

  /**
   * Convenience method for checking the runtime type of an Expression.
   * Returns true if conversion to derived type is possible. Otherwise
   * return false.
   */
   template<typename T> bool is() const {
     return (dynamic_cast<T*>(this) != nullptr);
   }

   /**
    * Convenience method for casting Expr base type to any one of its derived
    * types. Throws a std::logic_error if conversion is not possible.
    */
  template<typename T> T* as() const {
    T* casted_type = dynamic_cast<T*>(this);
    if (casted_type != nullptr) {
      return casted_type;
    } else {
      std::string error_message_prefix{"unable to cast Expr to "};
      throw std::logic_error(error_message_prefix + typeid(T).name());
    }
  }

  virtual std::string name() const override {
    return "statement";
  };

  virtual bool returns() const = 0;

  virtual Stmt::Kind getKind() const = 0;
};

class CompoundStmt : public Stmt {
private:
  DeclContext context_;
  std::vector<std::unique_ptr<Stmt>> stmts_;
public:

  DeclContext* getDeclContext() {
    return &context_;
  }

  std::string name() const override {
    return "compound-statement";
  };

  void setParentContext(DeclContext *parent) {
    context_.setParentContext(parent);
  }

  CompoundStmt(std::vector<std::unique_ptr<Stmt>> stmts)
  : stmts_{std::move(stmts)} {
    assert(std::find(stmts_.begin(), stmts_.end(), nullptr) == stmts_.end()
      && "precondition: stmts must not contain nullptr");
  }

  const std::vector<std::unique_ptr<Stmt>>& getStmts() const {
    return stmts_;
  };

  Stmt::Kind getKind() const override { return Kind::CompoundStmt;}

  bool returns() const override {
    for (auto &stmt: stmts_) {
      if (stmt->returns()) return true;
    }
    return false;
  }

  std::vector<TreeElement*> getChildren() const override {
    std::vector<TreeElement*> treeVector;
    for (auto &stmt: stmts_) {
      treeVector.push_back(stmt.get());
    }
    return treeVector;
  }
};

class ConditionalStmt : public Stmt {
private:
  DeclContext context;
  std::unique_ptr<LetDecl> declaration_;
  std::unique_ptr<Expr> condition_;
  std::unique_ptr<CompoundStmt> stmt_;

public:

  ConditionalStmt(std::unique_ptr<Expr> condition, std::unique_ptr<CompoundStmt> stmt)
  : condition_{std::move(condition)}, stmt_{std::move(stmt)} {
    assert(condition_ && "precondition: condition is required");
    assert(stmt_ && "precondition: statement is required");
  }

  ConditionalStmt(std::unique_ptr<LetDecl> declaration, std::unique_ptr<CompoundStmt> stmt)
  : declaration_{std::move(declaration)}, stmt_{std::move(stmt)} {
    assert(declaration_ && "precondition: declaration is required");
    assert(stmt_ && "precondition: statement is required");
  }

  DeclContext* getDeclContext() {
    return &context;
  }

  std::string name() const override {
    return "conditional-statement";
  };

  void setParentContext(DeclContext *parent) {
    context.setParentContext(parent);
  }

  LetDecl* getDeclaration() {
    return declaration_.get();
  }

  Expr* getCondition() {
      return condition_.get();
  }

  CompoundStmt* getBlock() {
    return stmt_.get();
  }

  Stmt::Kind getKind() const override { return Kind::ConditionalStmt;}

  bool returns() const override {
    return stmt_->returns();
  }

  std::vector<TreeElement*> getChildren() const override;

};

class ConditionalBlock : public Stmt {
private:
  std::vector<std::unique_ptr<Stmt>> stmts_;
public:
  ConditionalBlock(std::vector<std::unique_ptr<Stmt>> stmts)
  : stmts_{std::move(stmts)} {

    assert( std::find_if_not(stmts_.begin(), stmts_.end(), [](auto &stmt) {
      return dynamic_cast<ConditionalStmt*>(stmt.get()) != nullptr
          || dynamic_cast<CompoundStmt*>(stmt.get()) != nullptr;
      }) == stmts_.end()
    && "precondition: stmts must contain only ConditionalStmt or CompoundStmt");

  }

  std::vector<std::unique_ptr<Stmt>>& getStmts() {
    return stmts_;
  }

  Stmt::Kind getKind() const override {
    return Stmt::Kind::ConditionalBlock;
  }

  std::string name() const override {
    return "conditional-block-statement";
  };

  std::vector<TreeElement*> getChildren() const override {
    std::vector<TreeElement*> children;
    for (auto &stmt: stmts_) {
      children.push_back(stmt.get());
    }
    return children;
  }
  virtual bool returns() const override { return true; }
};

class WhileLoop : public Stmt {
private:
  DeclContext context_;
  std::unique_ptr<LetDecl> decl_;
  std::unique_ptr<Expr> condition_;
  std::unique_ptr<CompoundStmt> stmt_;

public:

  WhileLoop(std::unique_ptr<LetDecl> decl, std::unique_ptr<CompoundStmt> stmt)
  : decl_{std::move(decl)}, stmt_{std::move(stmt)} {
    assert(decl_ && "precondition: declaration must not be nullptr");
    assert(stmt_ && "precondition: statement is required");
  }

  WhileLoop(std::unique_ptr<Expr> condition, std::unique_ptr<CompoundStmt> stmt)
  : condition_{std::move(condition)}, stmt_{std::move(stmt)} {
    assert(condition_ && "precondition: condition must not be nullptr");
    assert(stmt_ && "precondition: stmt must not be nullptr");
  }

  bool returns() const override {
    return stmt_->returns();
  }

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
