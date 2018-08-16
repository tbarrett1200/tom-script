#ifndef AST_STMT_H
#define AST_STMT_H

#include "AST/DeclContext.h"
#include "AST/TreeElement.h"
#include "Basic/SourceCode.h"

#include <memory>

class ReturnStmt;
class ConditionalStmtList;

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

  virtual bool returns() const = 0;

  virtual Stmt::Kind getKind() const = 0;
};

class CompoundStmt : public Stmt {
private:
  DeclContext context;
  std::vector<std::unique_ptr<Stmt>> list;
public:

  DeclContext* getDeclContext() {
    return &context;
  }

  void setParentContext(DeclContext *parent) {
    context.setParentContext(parent);
  }

  CompoundStmt(std::vector<std::unique_ptr<Stmt>>&& l) : list{std::move(l)} {}

  const std::vector<std::unique_ptr<Stmt>>& getStmts() const {
    return list;
  };

  Stmt::Kind getKind() const { return Kind::CompoundStmt;}

  bool returns() const {
    for (auto &stmt: list) {
      if (stmt->returns()) return true;
    }
    return false;
  }

  std::vector<TreeElement*> getChildren() const {
    std::vector<TreeElement*> treeVector;
    for (auto &stmt: list) {
      treeVector.push_back(stmt.get());
    }
    return treeVector;
  }
};

class ConditionalStmt : public Stmt {
private:
  DeclContext context;
  std::unique_ptr<class LetDecl> declaration;
  std::unique_ptr<class Expr> condition;
  std::unique_ptr<CompoundStmt> stmt;

public:

  // Constructors
  ConditionalStmt(std::unique_ptr<class LetDecl> d, std::unique_ptr<CompoundStmt> s)
  : declaration{std::move(d)}, stmt{std::move(s)} {}

  DeclContext* getDeclContext() {
    return &context;
  }

  void setParentContext(DeclContext *parent) {
    context.setParentContext(parent);
  }

  LetDecl* getDeclaration() {
    return declaration.get();
  }

  Expr* getCondition() {
      return condition.get();
  }

  CompoundStmt* getBlock() {
    return stmt.get();
  }


  ConditionalStmt(std::unique_ptr<Expr> c, std::unique_ptr<CompoundStmt> s) : condition{std::move(c)}, stmt{std::move(s)} {
    if (!stmt) throw std::logic_error("violated precondition: statement is required");
  }

  Stmt::Kind getKind() const { return Kind::ConditionalStmt;}

  bool returns() const {
    return stmt->returns();
  }

  bool isElseStmt() const {
    return !condition;
  }

  std::vector<TreeElement*> getChildren() const;

};

class ConditionalBlock : public Stmt {
private:
  std::vector<std::unique_ptr<Stmt>> stmts;
public:
  ConditionalBlock(std::vector<std::unique_ptr<Stmt>> list): stmts{std::move(list)} {}

  std::vector<std::unique_ptr<Stmt>>& getStmts() {
    return stmts;
  }

  Stmt::Kind getKind() const {
    return Stmt::Kind::ConditionalBlock;
  }

  std::vector<TreeElement*> getChildren() const {
    std::vector<TreeElement*> children;
    for (auto &stmt: stmts) {
      children.push_back(stmt.get());
    }
    return children;
  }
  virtual bool returns() const  { return true; }
};

class WhileLoop : public Stmt {
private:
  DeclContext context_;
  std::unique_ptr<class LetDecl> declaration_;
  std::unique_ptr<class Expr> condition_;
  std::unique_ptr<CompoundStmt> stmt_;

public:

  WhileLoop(std::unique_ptr<Expr> c, std::unique_ptr<CompoundStmt> s): condition_{std::move(c)}, stmt_{std::move(s)} {
    if (!condition_) throw std::logic_error("violated precondition: condition is required");
    if (!stmt_) throw std::logic_error("violated precondition: statement is required");
  }

  bool returns() const {
    return stmt_->returns();
  }

  Stmt::Kind getKind() const { return Kind::WhileLoop;}

  std::vector<TreeElement*> getChildren() const;

  LetDecl* getDeclaration() {
    return declaration_.get();
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


class ReturnStmt : public Stmt{
private:
  std::unique_ptr<Expr> expr_;

public:
  ReturnStmt(std::unique_ptr<Expr> d): expr_{std::move(d)} {}

  bool returns() const {
    return true;
  }

  const Expr& getExpr() const {
    return *expr_;
  }

  Expr& getExpr() {
    return *expr_;
  }


  Stmt::Kind getKind() const { return Kind::ReturnStmt;}

  std::vector<TreeElement*> getChildren() const;
};

class ExprStmt : public Stmt {
private:
  std::unique_ptr<class Expr> expr_;

public:

  ExprStmt(std::unique_ptr<Expr> e): expr_{std::move(e)} {
    if (!expr_) throw std::logic_error("violated precondition: expr is required");
  }

  bool returns() const {
    return false;
  }

  Stmt::Kind getKind() const { return Kind::ExprStmt;}

  std::vector<TreeElement*> getChildren() const;

  const Expr* getExpr() const {
    return expr_.get();
  }

  Expr* getExpr() {
    return expr_.get();
  }

};


class DeclStmt : public Stmt {
private:
  std::unique_ptr<class Decl> decl_;

public:

  DeclStmt(std::unique_ptr<Decl> d): decl_{std::move(d)} {
    if (!decl_) throw std::logic_error("violated precondition: decl is required");
  }

  Stmt::Kind getKind() const { return Kind::DeclStmt;}

  std::vector<TreeElement*> getChildren() const;

  bool returns() const {
    return false;
  }

  const Decl* getDecl() const {
    return decl_.get();
  }

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
  CompilationUnit(std::vector<std::unique_ptr<Stmt>> stmts): stmts_{std::move(stmts)} {};

  Stmt::Kind getKind() const { return Kind::CompilationUnit; }

  DeclContext* getDeclContext() {
    return &context_;
  }

  std::vector<TreeElement*> getChildren() const;

  bool returns() const {
    return true;
  }

  const std::vector<std::unique_ptr<Stmt>>& getStmts() const {
    return stmts_;
  }
};

#endif
