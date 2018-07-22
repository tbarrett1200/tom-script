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
  virtual std::shared_ptr<Type> getType() const = 0;


  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }
};


class TypeAlias : public Decl {
public:
  Token name;
  std::shared_ptr<Type> type;

  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr> e);

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const {
    return type;
  }

  TypeAlias(Token n, std::shared_ptr<Type> t);
};

class VarDecl : public Decl {
public:
  Token name;
  std::shared_ptr<Type> type;
  std::shared_ptr<Expr> expr;

  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr>);

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const {
    return type;
  }

  VarDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e);
};

class LetDecl : public Decl {
public:
  Token name;
  std::shared_ptr<Type> type;
  std::shared_ptr<Expr> expr;

  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr>);

  std::vector<std::shared_ptr<TreeElement>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const {
    return type;
  }
  LetDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e);
};


class ParamDecl : public Decl {
private:
  Token name;
  std::shared_ptr<Type> type;
public:

  ParamDecl(Token n, std::shared_ptr<Type> t)
  : name{n}, type{t} {}

  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const {
    return type;
  }
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

};

/// A named, explicitly typed function
class FuncDecl : public Decl {
private:
  std::shared_ptr<FunctionType> fType;
  Token fName;
  std::vector<std::shared_ptr<ParamDecl>> fParams;
  std::shared_ptr<Type> fReturnType;
  std::shared_ptr<CompoundStmt> fStmt;

public:


  FuncDecl(Token n, std::vector<std::shared_ptr<ParamDecl>>&& p, std::shared_ptr<Type> t, std::shared_ptr<CompoundStmt> s)
  : fName{n}, fParams{p}, fReturnType{t}, fStmt{s} {
    std::vector<std::shared_ptr<Type>> paramTypes;
    for (auto param: fParams) {
      paramTypes.push_back(param->getType());
    }
    fType = std::make_shared<FunctionType>(std::move(paramTypes), fReturnType);
  }

  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const {
    return fType;
  }
  std::vector<std::shared_ptr<TreeElement>> getChildren() const;

};
#endif
