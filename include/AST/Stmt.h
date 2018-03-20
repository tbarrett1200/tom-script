#ifndef AST_STMT_H
#define AST_STMT_H

#import "AST/Matchable.h"
#import "AST/Expr.h"
#import "AST/Decl.h"

#import <memory>

class Stmt : virtual public Matchable {
public:
  enum class Kind {
    #define STMT(SELF, PARENT) SELF,
    #include "AST/Stmt.def"
    #undef STMT
  };
  virtual Stmt::Kind getKind() const = 0;
};


class StmtList : public NonTerminal {
public:
  std::shared_ptr<Stmt> element;
  std::shared_ptr<StmtList> list;

  StmtList(std::shared_ptr<Stmt> e, std::shared_ptr<StmtList> l)
    : element{move(e)}, list{move(l)} {}

  StmtList(std::vector<std::shared_ptr<Stmt>> l) {
    if (l.size() == 0) {
      throw std::runtime_error("type list must have at least one type");
    }
    if (l.size() == 1) {
      element = l[0];
      list = nullptr;
    } else {
      element = l[0];
      l.erase(l.begin());
      list = std::make_shared<StmtList>(l);
    }
  }

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    if (!list) return {element};
    else {
      auto children = list->getChildren();
      children.insert(children.begin(), element);
      return children;
    }
  }

  int size() const {
    if (!list) return 1;
    else return list->size()+1;
  }
};


class CompoundStmt : public Stmt, public NonTerminal {
public:
  std::shared_ptr<StmtList> list;

  Stmt::Kind getKind() const { return Kind::CompoundStmt;}

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {list};
  }

  CompoundStmt(std::shared_ptr<StmtList> l)
    : list{l} {}
};

class ConditionalStmt : public Stmt, public NonTerminal {
public:
  std::shared_ptr<Expr> condition;
  std::shared_ptr<CompoundStmt> stmt;

  Stmt::Kind getKind() const { return Kind::ConditionalStmt;}

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    if (!condition) { return {stmt}; }
    else return {condition, stmt};
  }

  ConditionalStmt(std::shared_ptr<Expr> c, std::shared_ptr<CompoundStmt> s)
    : condition{c}, stmt{s} {}
};


class ConditionalStmtList : public Stmt, public NonTerminal {
public:
  std::shared_ptr<ConditionalStmt> element;
  std::shared_ptr<ConditionalStmtList> list;

  ConditionalStmtList(std::shared_ptr<ConditionalStmt> e, std::shared_ptr<ConditionalStmtList> l)
    : element{move(e)}, list{move(l)} {}

  ConditionalStmtList(std::vector<std::shared_ptr<ConditionalStmt>> l) {
    if (l.size() == 0) {
      throw std::runtime_error("type list must have at least one type");
    }
    if (l.size() == 1) {
      element = l[0];
      list = nullptr;
    } else {
      element = l[0];
      l.erase(l.begin());
      list = std::make_shared<ConditionalStmtList>(l);
    }
  }

  Stmt::Kind getKind() const { return Kind::ConditionalStmtList; }

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    if (!list) return {element};
    else {
      auto children = list->getChildren();
      children.insert(children.begin(), element);
      return children;
    }
  }

  int size() const {
    if (!list) return 1;
    else return list->size()+1;
  }
};

class WhileLoop : public Stmt, public NonTerminal {
public:
  std::shared_ptr<Expr> condition;
  std::shared_ptr<CompoundStmt> stmt;


  Stmt::Kind getKind() const { return Kind::WhileLoop;}

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {condition, stmt};
  }

  WhileLoop(std::shared_ptr<Expr> c, std::shared_ptr<CompoundStmt> s)
    : condition{c}, stmt{s} {}
};


class ReturnStmt : public Stmt, public NonTerminal {
public:
  std::shared_ptr<Expr> expr;


  Stmt::Kind getKind() const { return Kind::ReturnStmt;}

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {expr};
  }

  ReturnStmt(std::shared_ptr<Expr> d)
    : expr{d} {}
};


class ExprStmt : public Stmt, public NonTerminal {
public:
  std::shared_ptr<Expr> expr;


  Stmt::Kind getKind() const { return Kind::ExprStmt;}

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {expr};
  }

  ExprStmt(std::shared_ptr<Expr> d)
    : expr{d} {}
};


class DeclStmt : public Stmt, public NonTerminal {
public:
  std::shared_ptr<Decl> decl;


  Stmt::Kind getKind() const { return Kind::DeclStmt;}

  std::vector<std::shared_ptr<Matchable>> getChildren() const {
    return {decl};
  }

  DeclStmt(std::shared_ptr<Decl> d)
    : decl{d} {}
};

#endif
