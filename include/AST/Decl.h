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

  virtual const char* location() const = 0;

  virtual ~Decl() = default;
  virtual Type* getType() const = 0;

  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }

  virtual void setParentContext(DeclContext *parent) = 0;

  Type* canonical_type() const {
    return getType()->getCanonicalType();
  }

  template <typename T> bool is() const {
    return dynamic_cast<T*>(this);
  }

  template <typename T> const T* as() const {
    return dynamic_cast<T*>(this);
  }

  template <typename T> T* as() const {
    return dynamic_cast<T*>(this);
  }

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

  const char* location() const override {
      return fName.location();
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

  TypeAlias(Token aName, Type* aType)
  :  fName{aName}, fType{aType} {}
};

/// Represents
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

  const char* location() const override {
      return fName.location();
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

  std::string name() const override {
    return "var-declaration";
  };

  virtual void setParentContext(DeclContext *parent) override {
    fParentContext = parent;
  }

  VarDecl(Token n, Type* t, std::unique_ptr<Expr> e)
  : fName{n}, fType{t}, fExpr{std::move(e)} {}
};


/// Represents
class UninitializedVarDecl : public Decl {
private:
  DeclContext* fParentContext;
  Token fName;
  Type* fType;

public:

  Decl::Kind getKind() const override {
    return Decl::Kind::UninitializedVarDecl;
  }
  StringRef getName() const override {
    return fName.lexeme();
  }
  Type* getType() const override {
    return fType;
  }

  const char* location() const override {
      return fName.location();
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

  UninitializedVarDecl(Token n, Type* t)
  : fName{n}, fType{t} {}
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

  const char* location() const override {
      return fName.location();
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

  std::string name() const override {
    return "let-declaration";
  };

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

  const char* location() const override {
      return fName.location();
  }

  Type* getType() const override {
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


class StructDecl : public Decl {
private:
  DeclContext* parent_context_;
  Token name_;
  StructType* type_;
public:

  StructDecl(Token name, StructType *type)
  : name_{name}, type_{type} {}

  Decl::Kind getKind() const override {
    return Decl::Kind::StructDecl;
  }
  StringRef getName() const override {
    return name_.lexeme();
  }

  const char* location() const override {
      return name_.location();
  }

  Type* getType() const override {
    return type_;
  }

  std::string name() const override {
    return "struct-declaration";
  };

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


/// A named, explicitly typed function
class FuncDecl : public Decl {
private:
  DeclContext fContext;
  FunctionType* fType;
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
    fType = FunctionType::getInstance(paramTypes, fReturnType, false);
  }

  const char* location() const override {
      return fName.location();
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

  Type* getType() const override {
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
  Type *type_;
public:

  BasicDecl(Token name, Type *type): name_{name}, type_{type} {}

  Decl::Kind getKind() const override {
    return Decl::Kind::BasicDecl;
  }

  StringRef getName() const override {
    return name_.lexeme();
  }

  const char* location() const override {
      return name_.location();
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

class ExternFuncDecl : public Decl {
private:
  DeclContext* parent_context_;
  Token name_;
  FunctionType *type_;
public:

  ExternFuncDecl(Token name, FunctionType *type): name_{name}, type_{type} {}

  Decl::Kind getKind() const override {
    return Decl::Kind::BasicDecl;
  }

  StringRef getName() const override {
    return name_.lexeme();
  }

  std::string name() const override {
    return "extern-func-decl";
  }
  const char* location() const override {
      return name_.location();
  }

  Type* getType() const override {
    return type_;
  }

  FunctionType *getFunctionType() const {
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
