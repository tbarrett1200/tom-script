#ifndef AST_DECL_H
#define AST_DECL_H

#include "Basic/SourceCode.h"
#include "Basic/Token.h"
#include "AST/TreeElement.h"
#include "AST/Expr.h"
#include "AST/Type.h"

#include <vector>

class CompoundStmt;
class FunctionType;
class DeclContext;

class Decl : public TreeElement {
public:
  enum class Kind {
    #define DECL(SELF, PARENT) SELF,
    #include "AST/Decl.def"
    #undef DECL
  };

  virtual Decl::Kind getKind() const = 0;
  virtual std::string getName() const = 0;

  SourceLocation getLocation() const {
    return {0, 0};
  }

  virtual ~Decl() = default;


  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }
};

class DeclName  : public TreeElement  {
public:
  Token token;
  std::string getLexeme() const;
  DeclName(Token n);
};

class TypeDecl : public Decl {
public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr> e);
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;

  TypeDecl(Token n);
};

class TypeAlias : public Decl {
public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<Type> type;

  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr> e);

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;


  TypeAlias(Token n, std::shared_ptr<Type> t);
};

class VarDecl : public Decl {
public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<Type> type;
  std::shared_ptr<Expr> expr;

  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr>);

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;


  VarDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e);
};

class LetDecl : public Decl {
public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<Type> type;
  std::shared_ptr<Expr> expr;

  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr>);

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;

  LetDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e);
};


class ParamDecl : public Decl {
public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<Type> type;

  ParamDecl(Token n, std::shared_ptr<Type> t);
  Decl::Kind getKind() const;
  std::string getName() const;
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;


};

class ParamDeclList  : public TreeElement  {
public:
  std::shared_ptr<ParamDecl> element;
  std::shared_ptr<ParamDeclList> list;

  ParamDeclList(std::shared_ptr<ParamDecl> e, std::shared_ptr<ParamDeclList> l);
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

};

/// A named, explicitly typed function
class FuncDecl : public Decl {
public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<ParamDeclList> params;
  std::shared_ptr<Type> returnType;
  std::shared_ptr<FunctionType> type;
  std::shared_ptr<CompoundStmt> stmt;


  FuncDecl(Token n, std::shared_ptr<ParamDeclList> t, std::shared_ptr<Type>, std::shared_ptr<CompoundStmt> s);
  Decl::Kind getKind() const;
  std::string getName() const;

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

};
#endif
