#ifndef AST_STMT_H
#define AST_STMT_H

#include "AST/TreeElement.h"
#include "AST/Expr.h"
#include "AST/Decl.h"
#include "Basic/SourceCode.h"

#include <memory>

class ReturnStmt;
class ConditionalStmtList;

class Stmt : virtual public TreeElement {
public:
  enum class Kind {
    #define STMT(SELF, PARENT) SELF,
    #include "AST/Stmt.def"
    #undef STMT
  };

  virtual ~Stmt() = default;

  SourceLocation getLocation() const {
    return {0, 0};
  }
  virtual bool returns() const = 0;
  virtual Stmt::Kind getKind() const = 0;
};


class StmtList : public TreeElement {
public:
  std::shared_ptr<Stmt> element;
  std::shared_ptr<StmtList> list;

  // Constructors
  StmtList(std::shared_ptr<Stmt> e, std::shared_ptr<StmtList> l);
  StmtList(std::vector<std::shared_ptr<Stmt>> l);

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  template <typename T> bool has() const;
  bool returns() const;
  int size() const;
};

class CompoundStmt : public Stmt {
public:
  std::shared_ptr<StmtList> list;

  // Constructors
  CompoundStmt(std::shared_ptr<StmtList> l);

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  Stmt::Kind getKind() const;
  bool returns() const;
};

class ConditionalStmt : public Stmt {
public:
  std::shared_ptr<Expr> condition;
  std::shared_ptr<CompoundStmt> stmt;

  // Constructors
  ConditionalStmt(std::shared_ptr<Expr> c, std::shared_ptr<CompoundStmt> s);

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
  std::shared_ptr<Expr> condition;
  std::shared_ptr<CompoundStmt> stmt;

  // Constructors
  WhileLoop(std::shared_ptr<Expr> c, std::shared_ptr<CompoundStmt> s);

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
  std::shared_ptr<StackPointer> location;

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
  std::shared_ptr<Expr> expr;

  // Constructors
  ExprStmt(std::shared_ptr<Expr> d);

  // Stmt Overrides
  Stmt::Kind getKind() const;

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  bool returns() const;
};


class DeclStmt : public Stmt {
public:
  std::shared_ptr<Decl> decl;

  // Constructors
  DeclStmt(std::shared_ptr<Decl> d);

  // Stmt Overrides
  Stmt::Kind getKind() const;

  // TreeElement
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

  // Utility Methods
  bool returns() const;
};

#endif
