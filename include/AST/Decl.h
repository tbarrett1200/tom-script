#ifndef AST_DECL_H
#define AST_DECL_H

#include "Basic/SourceCode.h"
#include "Basic/Token.h"
#include "AST/TreeElement.h"
#include "AST/DeclContext.h"
#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/Stmt.h"

#include <vector>
#include <memory>

class CompoundStmt;
class FunctionType;

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

  virtual void setDeclContext(DeclContext *parent) = 0;
  virtual DeclContext* getDeclContext() const = 0;
};


class TypeAlias : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  std::shared_ptr<Type> fType;

public:
  std::vector<std::shared_ptr<TreeElement>> getChildren() const override {
    return {};
  }

  Decl::Kind getKind() const override{
    return Decl::Kind::TypeAlias;
  }

  std::string getName() const override {
    return fName.lexeme;
  }

  std::shared_ptr<Type> getType() const override {
    return fType;
  }

  virtual DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual void setDeclContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  TypeAlias(Token aName, std::shared_ptr<Type> aType)
  :  fName{aName}, fType{aType} {}
};

class VarDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  std::shared_ptr<Type> fType;
  std::shared_ptr<Expr> fExpr;

public:

  std::shared_ptr<Expr> getExpr() const {
    return fExpr;
  }

  std::vector<std::shared_ptr<TreeElement>> getChildren() const override {
    return {fExpr};
  }

  Decl::Kind getKind() const override {
    return Decl::Kind::VarDecl;
  }
  std::string getName() const override {
    return fName.lexeme;
  }
  std::shared_ptr<Type> getType() const override {
    return fType;
  }

  virtual DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual void setDeclContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  VarDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e)
  : fName{n}, fType{t}, fExpr{e} {}
};


class LetDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  std::shared_ptr<Type> fType;
  std::shared_ptr<Expr> fExpr;
public:

  std::vector<std::shared_ptr<TreeElement>> getChildren() const override {
    return {fExpr};
  }

  Decl::Kind getKind() const override {
    return Decl::Kind::LetDecl;
  }

  std::string getName() const override {
    return fName.lexeme;
  }
  std::shared_ptr<Type> getType() const override {
    return fType;
  }

  std::shared_ptr<Expr> getExpr() const {
    return fExpr;
  }
  virtual DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual void setDeclContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  LetDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e)
  : fName{n}, fType{t}, fExpr{e} {}
};


class ParamDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  std::shared_ptr<Type> fType;
public:

  ParamDecl(Token n, std::shared_ptr<Type> t)
  : fName{n}, fType{t} {}

  Decl::Kind getKind() const override {
    return Decl::Kind::ParamDecl;
  }
  std::string getName() const override {
    return fName.lexeme;
  }

  std::shared_ptr<Type> getType() const override {
    return fType;
  }

  virtual DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual void setDeclContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  std::vector<std::shared_ptr<TreeElement>> getChildren() const override {
    return {};
  }

};

/// A named, explicitly typed function
class FuncDecl : public Decl {
private:
  std::unique_ptr<DeclContext> fContext;
  std::shared_ptr<FunctionType> fType;
  Token fName;
  std::vector<std::shared_ptr<ParamDecl>> fParams;
  std::shared_ptr<Type> fReturnType;
  std::shared_ptr<CompoundStmt> fStmt;
public:

  FuncDecl(Token n, std::vector<std::shared_ptr<ParamDecl>>&& p, std::shared_ptr<Type> t, std::shared_ptr<CompoundStmt> s)
  : fContext{std::make_unique<DeclContext>(DeclContext::getGlobalContext())}
  , fName{n}
  , fParams{p}
  , fReturnType{t}
  , fStmt{s} {

    std::vector<std::shared_ptr<Type>> paramTypes;

    for (auto param: fParams) {
      fContext->addDecl(param.get());
      paramTypes.push_back(param->getType());
    }

    for (auto stmt: s->list) {
      DeclStmt* declStmt = dynamic_cast<DeclStmt*>(stmt.get());
      if (declStmt) {
        Decl* decl = dynamic_cast<Decl*>(declStmt->decl.get());
        if (decl) {
          fContext->addDecl(decl);
        }
      }
    }
    fType = std::make_shared<FunctionType>(std::move(paramTypes), fReturnType);
  }

  std::shared_ptr<CompoundStmt> getBlockStmt() const {
    return fStmt;
  };

  Decl::Kind getKind() const override {
    return Decl::Kind::FuncDecl;
  }

  std::string getName() const override {
    return fName.lexeme;
  }

  std::shared_ptr<Type> getType() const override{
    return fType;
  }

  virtual DeclContext* getDeclContext() const override {
    return fContext.get();
  }

  virtual void setDeclContext(DeclContext *parent) override {
    fContext->setParentContext(parent);
  }

  std::vector<std::shared_ptr<TreeElement>> getChildren() const override {
    std::vector<std::shared_ptr<TreeElement>> children;
    for (auto param: fParams) {
      children.push_back(param);
    }
    children.push_back(fStmt);
    return children;
  }
  const std::vector<std::shared_ptr<ParamDecl>>& getParams() const {
    return fParams;
  };

};

#endif
