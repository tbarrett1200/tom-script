#ifndef AST_STMT_H
#define AST_STMT_H

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

  SourceLocation getLocation() const {
    return {0, 0};
  }
  virtual bool returns() const = 0;

  virtual Stmt::Kind getKind() const = 0;
};

class CompoundStmt : public Stmt {
public:
  std::vector<std::shared_ptr<Stmt>> list;

  // Constructors
  CompoundStmt(std::vector<std::shared_ptr<Stmt>>&& l);

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  Stmt::Kind getKind() const;
  bool returns() const;
};

class ConditionalStmt : public Stmt {
public:
  std::shared_ptr<class Expr> condition;
  std::shared_ptr<CompoundStmt> stmt;

  // Constructors
  ConditionalStmt(std::shared_ptr<class Expr> c, std::shared_ptr<CompoundStmt> s);

  // Stmt Overrides
  Stmt::Kind getKind() const;

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  bool returns() const;
  bool isElseStmt() const;
};


class ConditionalStmtList : public Stmt {
public:
  std::shared_ptr<ConditionalStmt> element;
  std::shared_ptr<ConditionalStmtList> list;

  // Constructors
  ConditionalStmtList(std::shared_ptr<ConditionalStmt> e, std::shared_ptr<ConditionalStmtList> l);
  ConditionalStmtList(std::vector<std::shared_ptr<ConditionalStmt>> l);

  // Stmt Overrides
  Stmt::Kind getKind() const;

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  bool hasElseStmt() const;
  bool returns() const;
  int size() const;

};

class WhileLoop : public Stmt {
public:
  std::shared_ptr<class Expr> condition;
  std::shared_ptr<CompoundStmt> stmt;

  // Constructors
  WhileLoop(std::shared_ptr<class Expr> c, std::shared_ptr<CompoundStmt> s);

  // Stmt Overrides
  Stmt::Kind getKind() const;

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  bool returns() const;
};


class ReturnStmt : public Stmt{
public:
  std::shared_ptr<Expr> expr;

  // Constructors
  ReturnStmt(std::shared_ptr<Expr> d);

  // Stmt Overrides
  Stmt::Kind getKind() const;

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  bool returns() const;
};


class ExprStmt : public Stmt {
public:
  std::shared_ptr<class Expr> expr;

  // Constructors
  ExprStmt(std::shared_ptr<class Expr> d);

  // Stmt Overrides
  Stmt::Kind getKind() const;

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  bool returns() const;
};


class DeclStmt : public Stmt {
public:
  std::shared_ptr<class Decl> decl;

  // Constructors
  DeclStmt(std::shared_ptr<class Decl> d);

  // Stmt Overrides
  Stmt::Kind getKind() const;

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  bool returns() const;
};

class CompilationUnit : public Stmt {
private:
  std::vector<std::shared_ptr<Stmt>> stmts;
public:
  CompilationUnit(std::vector<std::shared_ptr<Stmt>> s): stmts{s} {};

  Stmt::Kind getKind() const { return Kind::CompilationUnit; }

  std::vector<std::shared_ptr<TreeElement>> getChildren() const {
    std::vector<std::shared_ptr<TreeElement>> children;
    for (auto stmt: stmts) {
      children.push_back(stmt);
    }
    return children;
  }

  bool returns() const {
    return true;
  }

  std::vector<std::shared_ptr<Stmt>>& getStmts() {
    return stmts;
  }
};

#endif
