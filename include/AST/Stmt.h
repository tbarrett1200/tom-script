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
  std::vector<std::shared_ptr<Stmt>> list;
public:

  DeclContext* getDeclContext() {
    return &context;
  }

  void setParentContext(DeclContext *parent) {
    context.setParentContext(parent);
  }

  CompoundStmt(std::vector<std::shared_ptr<Stmt>>&& l) : list{std::move(l)} {}

  const std::vector<std::shared_ptr<Stmt>>& getStmts() const {
    return list;
  };

  Stmt::Kind getKind() const { return Kind::CompoundStmt;}

  bool returns() const {
    for (auto stmt: list) {
      if (stmt->returns()) return true;
    }
    return false;
  }

  std::vector<std::shared_ptr<TreeElement>> getChildren() const {
    std::vector<std::shared_ptr<TreeElement>> treeVector;
    std::copy(list.begin(), list.end(), std::back_inserter(treeVector));
    return treeVector;
  }
};

class ConditionalStmt : public Stmt {
private:
  DeclContext context;
  std::shared_ptr<class LetDecl> declaration;
  std::shared_ptr<class Expr> condition;
  std::shared_ptr<CompoundStmt> stmt;

public:

  // Constructors
  ConditionalStmt(std::shared_ptr<class LetDecl> d, std::shared_ptr<CompoundStmt> s)
  : declaration{d}, stmt{s} {}

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


  ConditionalStmt(std::shared_ptr<Expr> c, std::shared_ptr<CompoundStmt> s) : condition{c}, stmt{s} {
    if (!s) throw std::logic_error("violated precondition: statement is required");
  }

  Stmt::Kind getKind() const { return Kind::ConditionalStmt;}

  bool returns() const {
    return stmt->returns();
  }

  bool isElseStmt() const {
    return !condition;
  }

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

};

class ConditionalBlock : public Stmt {
private:
  std::vector<std::shared_ptr<Stmt>> stmts;
public:
  ConditionalBlock(std::vector<std::shared_ptr<Stmt>> list): stmts{list} {}

  std::vector<std::shared_ptr<Stmt>>& getStmts() {
    return stmts;
  }

  Stmt::Kind getKind() const {
    return Stmt::Kind::ConditionalBlock;
  }

  std::vector<std::shared_ptr<TreeElement>> getChildren() const {
    std::vector<std::shared_ptr<TreeElement>> children;
    for (auto stmt: stmts) {
      children.push_back(stmt);
    }
    return children;
  }
  virtual bool returns() const  { return true; }
};

class WhileLoop : public Stmt {
private:
  DeclContext context_;
  std::shared_ptr<class LetDecl> declaration_;
  std::shared_ptr<class Expr> condition_;
  std::shared_ptr<CompoundStmt> stmt_;

public:

  WhileLoop(std::shared_ptr<Expr> c, std::shared_ptr<CompoundStmt> s): condition_{c}, stmt_{s} {
    if (!c) throw std::logic_error("violated precondition: condition is required");
    if (!s) throw std::logic_error("violated precondition: statement is required");
  }

  bool returns() const {
    return stmt_->returns();
  }

  Stmt::Kind getKind() const { return Kind::WhileLoop;}

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

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
  std::shared_ptr<Expr> expr_;

public:
  ReturnStmt(std::shared_ptr<Expr> d): expr_{d} {}

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

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
};

class ExprStmt : public Stmt {
private:
  std::shared_ptr<class Expr> expr_;

public:

  ExprStmt(std::shared_ptr<Expr> e): expr_{e} {
    if (!e) throw std::logic_error("violated precondition: expr is required");
  }

  bool returns() const {
    return false;
  }

  Stmt::Kind getKind() const { return Kind::ExprStmt;}

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  const Expr* getExpr() const {
    return expr_.get();
  }

  Expr* getExpr() {
    return expr_.get();
  }

};


class DeclStmt : public Stmt {
private:
  std::shared_ptr<class Decl> decl_;

public:

  DeclStmt(std::shared_ptr<Decl> d): decl_{d} {
    if (!d) throw std::logic_error("violated precondition: decl is required");
  }

  Stmt::Kind getKind() const { return Kind::DeclStmt;}

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

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

class CompilationUnit : public Stmt {
private:
  std::vector<std::shared_ptr<Stmt>> stmts;
  DeclContext context;
public:
  CompilationUnit(std::vector<std::shared_ptr<Stmt>> s): stmts{s} {};

  Stmt::Kind getKind() const { return Kind::CompilationUnit; }

  DeclContext* getDeclContext() {
    return &context;
  }

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  bool returns() const {
    return true;
  }

  const std::vector<std::shared_ptr<Stmt>>& getStmts() const {
    return stmts;
  }
};

#endif
