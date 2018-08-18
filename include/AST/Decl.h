#ifndef AST_DECL_H
#define AST_DECL_H

#include <vector>
#include <memory>

#include "Basic/SourceCode.h"
#include "Basic/Token.h"

#include "AST/TreeElement.h"
#include "AST/DeclContext.h"
#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/Stmt.h"

class CompoundStmt;
class FunctionType;

/// Represents a declaration, which can either be of a type or a variable.
/// All declarations have a name, which can be searched for in a DeclContext,
/// and a type, which must be included in the declaration. There is currently
/// no automatic type deduction.
class Decl : public TreeElement {
public:
  enum class Kind {
    #define DECL(SELF, PARENT) SELF,
    #include "AST/Decl.def"
    #undef DECL
  };

  virtual Decl::Kind getKind() const = 0;
  virtual StringRef getName() const = 0;

  virtual std::string name() const override {
    return "decl";
  };

  virtual std::vector<std::pair<std::string, std::string>> getAttributes() const override {
    return {
      {"name", getName().str()}
    , {"type", getType()->toString()}
    };
  }

  virtual ~Decl() = default;
  virtual const Type* getType() const = 0;

  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }

  virtual void setParentContext(DeclContext *parent) = 0;

  /// Returns a the top-most active context for this declaration
  virtual const DeclContext* getDeclContext() const = 0;
  /// Returns a the top-most active context for this declaration
  virtual DeclContext* getDeclContext() = 0;
};

/// Represents an alias to an existing type.
class TypeAlias : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  const Type* fType;

public:

  Decl::Kind getKind() const override{
    return Decl::Kind::TypeAlias;
  }

  StringRef getName() const override {
    return fName.lexeme();
  }

  const Type* getType() const override {
    return fType;
  }

  virtual const DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual DeclContext* getDeclContext() override {
    return fParentContext;
  }

  std::string name() const override {
    return "type-alias-declaration";
  };

  virtual void setParentContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  TypeAlias(Token aName, const Type* aType)
  :  fName{aName}, fType{aType} {}
};

/// Represents
class VarDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  const Type* fType;
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
  const Type* getType() const override {
    return fType;
  }

  virtual const DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual DeclContext* getDeclContext() override {
    return fParentContext;
  }

  std::string name() const override {
    return "var-declaration";
  };

  virtual void setParentContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  VarDecl(Token n, const Type* t, std::unique_ptr<Expr> e)
  : fName{n}, fType{t}, fExpr{std::move(e)} {}
};


/// Represents
class UninitializedVarDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  const Type* fType;

public:

  Decl::Kind getKind() const override {
    return Decl::Kind::UninitializedVarDecl;
  }
  StringRef getName() const override {
    return fName.lexeme();
  }
  const Type* getType() const override {
    return fType;
  }

  virtual const DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  virtual DeclContext* getDeclContext() override {
    return fParentContext;
  }

  std::string name() const override {
    return "uninitialized-var-declaration";
  };

  virtual void setParentContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  UninitializedVarDecl(Token n, const Type* t)
  : fName{n}, fType{t} {}
};


class LetDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  const Type *fType;
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
  const Type* getType() const override {
    return fType;
  }

  Expr& getExpr() const {
    return *fExpr;
  }
  virtual const DeclContext* getDeclContext() const override {
    return fParentContext;
  }

  std::string name() const override {
    return "let-declaration";
  };

  virtual DeclContext* getDeclContext() override {
    return fParentContext;
  }

  virtual void setParentContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  LetDecl(Token n, const Type* t, std::unique_ptr<Expr> e)
  : fName{n}, fType{t}, fExpr{std::move(e)} {}
};


class ParamDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  const Type* fType;
public:

  ParamDecl(Token n, const Type *t)
  : fName{n}, fType{t} {}

  Decl::Kind getKind() const override {
    return Decl::Kind::ParamDecl;
  }
  StringRef getName() const override {
    return fName.lexeme();
  }

  const Type* getType() const override {
    return fType;
  }

  std::string name() const override {
    return "parameter-declaration";
  };

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
  const FunctionType* fType;
  Token fName;
  std::vector<std::unique_ptr<ParamDecl>> fParams;
  const Type *fReturnType;
  std::unique_ptr<CompoundStmt> fStmt;
public:

  FuncDecl(Token n, std::vector<std::unique_ptr<ParamDecl>> p, const Type *t, std::unique_ptr<CompoundStmt> s)
  : fName{n}
  , fParams{std::move(p)}
  , fReturnType{t}
  , fStmt{std::move(s)} {

    std::vector<const Type*> paramTypes;
    for (auto &param: fParams) {
      paramTypes.push_back(param->getType());
    }
    fType = FunctionType::getInstance(paramTypes, fReturnType);
  }

  CompoundStmt& getBlockStmt() const {
    return *fStmt;
  };

  std::string name() const override {
    return "function-declaration";
  };

  Decl::Kind getKind() const override {
    return Decl::Kind::FuncDecl;
  }

  StringRef getName() const override {
    return fName.lexeme();
  }

  const Type* getType() const override {
    return fType;
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
  const Type *type_;
public:

  BasicDecl(Token name, const Type *type): name_{name}, type_{type} {}

  Decl::Kind getKind() const override {
    return Decl::Kind::BasicDecl;
  }

  StringRef getName() const override {
    return name_.lexeme();
  }

  const Type* getType() const override {
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
