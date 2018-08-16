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
  virtual StringRef getName() const = 0;

  virtual ~Decl() = default;
  virtual Type* getType() const = 0;

  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }

  virtual void setParentContext(DeclContext *parent) = 0;

  /// Returns a the top-most active context for this declaration
  virtual const DeclContext* getDeclContext() const = 0;
  /// Returns a the top-most active context for this declaration
  virtual DeclContext* getDeclContext() = 0;
};


class TypeAlias : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  Type* fType;

public:

  Decl::Kind getKind() const override{
    return Decl::Kind::TypeAlias;
  }

  StringRef getName() const override {
    return fName.lexeme();
  }

  Type* getType() const override {
    return fType;
  }

  virtual const DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual DeclContext* getDeclContext() override {
    return fParentContext;
  }

  virtual void setParentContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  TypeAlias(Token aName, Type* aType)
  :  fName{aName}, fType{aType} {}
};

class VarDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  Type* fType;
  std::unique_ptr<Expr> fExpr;

public:

  Expr& getExpr() const {
    return *fExpr;
  }

  std::vector<TreeElement*> getChildren() const override {
    return {fExpr.get()};
  }

  Decl::Kind getKind() const override {
    return Decl::Kind::VarDecl;
  }
  StringRef getName() const override {
    return fName.lexeme();
  }
  Type* getType() const override {
    return fType;
  }

  virtual const DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual DeclContext* getDeclContext() override {
    return fParentContext;
  }

  virtual void setParentContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  VarDecl(Token n, Type* t, std::unique_ptr<Expr> e)
  : fName{n}, fType{t}, fExpr{std::move(e)} {}
};


class LetDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  Type *fType;
  std::unique_ptr<Expr> fExpr;
public:

  std::vector<TreeElement*> getChildren() const override {
    return {fExpr.get()};
  }

  Decl::Kind getKind() const override {
    return Decl::Kind::LetDecl;
  }

  StringRef getName() const override {
    return fName.lexeme();
  }
  Type* getType() const override {
    return fType;
  }

  Expr& getExpr() const {
    return *fExpr;
  }
  virtual const DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual DeclContext* getDeclContext() override {
    return fParentContext;
  }

  virtual void setParentContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  LetDecl(Token n, Type* t, std::unique_ptr<Expr> e)
  : fName{n}, fType{t}, fExpr{std::move(e)} {}
};


class ParamDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  Type* fType;
public:

  ParamDecl(Token n, Type *t)
  : fName{n}, fType{t} {}

  Decl::Kind getKind() const override {
    return Decl::Kind::ParamDecl;
  }
  StringRef getName() const override {
    return fName.lexeme();
  }

  Type* getType() const override {
    return fType;
  }

  virtual const DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual DeclContext* getDeclContext() override {
    return fParentContext;
  }

  virtual void setParentContext(DeclContext *parent) override {
    fParentContext = parent;
  }

};

/// A named, explicitly typed function
class FuncDecl : public Decl {
private:
  DeclContext fContext;
  std::unique_ptr<FunctionType> fType;
  Token fName;
  std::vector<std::unique_ptr<ParamDecl>> fParams;
  Type *fReturnType;
  std::unique_ptr<CompoundStmt> fStmt;
public:

  FuncDecl(Token n, std::vector<std::unique_ptr<ParamDecl>> p, Type *t, std::unique_ptr<CompoundStmt> s)
  : fName{n}
  , fParams{std::move(p)}
  , fReturnType{t}
  , fStmt{std::move(s)} {

    std::vector<Type*> paramTypes;
    for (auto &param: fParams) {
      paramTypes.push_back(param->getType());
    }
    fType = std::make_unique<FunctionType>(std::move(paramTypes), fReturnType);
  }

  CompoundStmt& getBlockStmt() const {
    return *fStmt;
  };

  Decl::Kind getKind() const override {
    return Decl::Kind::FuncDecl;
  }

  StringRef getName() const override {
    return fName.lexeme();
  }

  Type* getType() const override {
    return fType.get();
  }

  virtual const DeclContext* getDeclContext() const override {
    return &fContext;
  }

  virtual DeclContext* getDeclContext() override {
    return &fContext;
  }

  virtual void setParentContext(DeclContext *parent) override {
    fContext.setParentContext(parent);
  }

  std::vector<TreeElement*> getChildren() const override {
    std::vector<TreeElement*> children;
    for (auto &param: fParams) {
      children.push_back(param.get());
    }
    children.push_back(fStmt.get());
    return children;
  }

  const std::vector<std::unique_ptr<ParamDecl>>& getParams() const {
    return fParams;
  };

};

class BasicDecl : public Decl {
private:
  DeclContext* parent_context_;
  Token name_;
  Type *type_;
public:

  BasicDecl(Token name, Type *type): name_{name}, type_{type} {}

  Decl::Kind getKind() const override {
    return Decl::Kind::BasicDecl;
  }

  StringRef getName() const override {
    return name_.lexeme();
  }

  Type* getType() const override {
    return type_;
  }

  virtual const DeclContext* getDeclContext() const override {
    return parent_context_;
  }

  virtual DeclContext* getDeclContext() override {
    return parent_context_;
  }

  virtual void setParentContext(DeclContext *parent) override {
    parent_context_ = parent;
  }
};

#endif
