#include "Parse/Token.h"
#include "AST/Type.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Decl.h"
#include "AST/Matchable.h"
#include "AST/Decl.h"

#include <sstream>

std::ostream& operator<<(std::ostream& os, Decl& x) {

  if (x.getKind() == Decl::Kind::TypeAlias) {
    auto t = x.getType();
    auto f = x.getContext()->getFundamentalType(t);
    os << x.getName() << " => " << t;
    if (!(*f == *t)) os << " => " << f;
  } else {
    auto t = x.getType();
    os << x.getName() << ": " << t;
  }
  return os;
}

std::shared_ptr<Decl> Decl::make(std::shared_ptr<Decl> d, std::function<void(RuntimeStack& vector)> f) {
  d->func = f;
  return d;
}

//=--------------------------------------------------------------------------=//
// OperatorExpr
//=--------------------------------------------------------------------------=//

DeclName::DeclName(Token n) : token{n} {};

std::string DeclName::getLexeme() const {
  return token.lexeme;
}

//=--------------------------------------------------------------------------=//
// TypeDecl
//=--------------------------------------------------------------------------=//

std::shared_ptr<Expr> TypeDecl::getExpr() const { return nullptr; };
void setExpr(std::shared_ptr<Expr> e) {};

std::vector<std::shared_ptr<Matchable>> TypeDecl::getChildren() const { return {name}; }
Decl::Kind TypeDecl::getKind() const {return Kind::TypeDecl; }
std::string TypeDecl::getName() const { return name->token.lexeme; }
std::shared_ptr<Type> TypeDecl::getType() const { return nullptr; }
DeclarationContext* TypeDecl::getContext() const { return context;}
void TypeDecl::setContext(DeclarationContext* c) {context = c;}
TypeDecl::TypeDecl(Token n) : name{std::make_shared<DeclName>(n)} {}

//=--------------------------------------------------------------------------=//
// TypeAlias
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<Matchable>> TypeAlias::getChildren() const {
  return {name, type};
}
std::shared_ptr<Expr> TypeAlias::getExpr() const { return nullptr; };
void TypeAlias::setExpr(std::shared_ptr<Expr> e) {};
Decl::Kind TypeAlias::getKind() const { return Kind::TypeAlias; }
std::string TypeAlias::getName() const { return name->getLexeme(); }
std::shared_ptr<Type> TypeAlias::getType() const { return type; };
DeclarationContext* TypeAlias::getContext() const { return context;}
void TypeAlias::setContext(DeclarationContext* c) {context = c;}
TypeAlias::TypeAlias(Token n, std::shared_ptr<Type> t)
: name{new DeclName{n}}, type{move(t)} {}

//=--------------------------------------------------------------------------=//
// VarDecl
//=--------------------------------------------------------------------------=//

/* Returns a vector of children for easy traversal */
std::vector<std::shared_ptr<Matchable>> VarDecl::getChildren() const {
  return {name, type, expr};
}

std::shared_ptr<Expr> VarDecl::getExpr() const { return expr; }

Decl::Kind VarDecl::getKind() const { return Kind::VarDecl; }
std::string VarDecl::getName() const { return name->getLexeme(); }
std::shared_ptr<Type> VarDecl::getType() const {
  return type;
}



DeclarationContext* VarDecl::getContext() const { return context; }
void VarDecl::setContext(DeclarationContext* c) { context = c; }
void VarDecl::setExpr(std::shared_ptr<Expr> e) {
  expr = e;
}

VarDecl::VarDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e)
: name{new DeclName{n}}, type{move(t)}, expr{move(e)} {}

//=--------------------------------------------------------------------------=//
// LetDecl
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<Matchable>> LetDecl::getChildren() const {
  return {name, type, expr};
}

Decl::Kind LetDecl::getKind() const { return Kind::LetDecl; }
std::string LetDecl::getName() const { return name->getLexeme(); }

void LetDecl::setExpr(std::shared_ptr<Expr> e) {
  expr = e;
}
std::shared_ptr<Expr> LetDecl::getExpr() const { return expr; }

DeclarationContext* LetDecl::getContext() const { return context; }
void LetDecl::setContext(DeclarationContext* c) { context = c; }

std::shared_ptr<Type> LetDecl::getType() const {
  return type;
};

LetDecl::LetDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e)
: name{new DeclName{n}}, type{move(t)}, expr{move(e)} {
  if (!expr) {
    throw std::domain_error("let decl must specify type");
  }
}

//=--------------------------------------------------------------------------=//
// ParamDecl
//=--------------------------------------------------------------------------=//
ParamDecl::ParamDecl(Token p, Token s, std::shared_ptr<Type> t)
: primary{std::make_shared<DeclName>(p)},
  secondary{std::make_shared<DeclName>(s)},
  type{t} {}

Decl::Kind ParamDecl::getKind() const { return Decl::Kind::ParamDecl; }
std::string ParamDecl::getName() const { return secondary->getLexeme(); }
std::shared_ptr<Type> ParamDecl::getType() const {
  if (primary->token.lexeme == "_") return type;
  else return std::make_shared<LabeledType>(std::make_shared<TypeLabel>(primary->token), type);
}

std::vector<std::shared_ptr<Matchable>> ParamDecl::getChildren() const {
  return { primary, secondary, type };
}

// Declaration Context Management
DeclarationContext* ParamDecl::getContext() const { return context; }
void ParamDecl::setContext(DeclarationContext* c) { context = c; }

//=--------------------------------------------------------------------------=//
// ParamDeclList
//=--------------------------------------------------------------------------=//

  ParamDeclList::ParamDeclList(std::shared_ptr<ParamDecl> e, std::shared_ptr<ParamDeclList> l)
  : element{e}, list{l} {
    if (!e) throw std::logic_error("violated precondition: element is required");
  }

  std::shared_ptr<TypeList> ParamDeclList::getTypeList() const {
    return std::make_shared<TypeList>(element->getType(), list ? list->getTypeList(): nullptr);
  }

  std::vector<std::shared_ptr<Matchable>> ParamDeclList::getChildren() const {
    return { element, list };
  }
  void ParamDeclList::setContext(DeclarationContext* c) {
    element->setContext(c);
    if (list) list->setContext(c);
  }

//=--------------------------------------------------------------------------=//
// FuncDecl
//=--------------------------------------------------------------------------=//

std::vector<std::shared_ptr<Matchable>> FuncDecl::getChildren() const {
  if (!stmt) return {name, type};
  else return {name, type, stmt};
}

Decl::Kind FuncDecl::getKind() const { return Kind::FuncDecl; }
std::string FuncDecl::getName() const { return name->getLexeme(); }

std::shared_ptr<Type> FuncDecl::getType() const {
  return type;
};

DeclarationContext* FuncDecl::getContext() const { return context.get(); }
void FuncDecl::setContext(DeclarationContext* c) { context->setParent(c); }

FuncDecl::FuncDecl(Token n, std::shared_ptr<ParamDeclList> p, std::shared_ptr<Type> r, std::shared_ptr<CompoundStmt> s)
: name{new DeclName{n}}, params{p}, returnType{r},
  type{new FunctionType(p ? p->getTypeList() : nullptr, r)}, stmt{s} {}
