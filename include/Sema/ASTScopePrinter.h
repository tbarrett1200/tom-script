#ifndef AST_SCOPE_PRINTER
#define AST_SCOPE_PRINTER

#include "AST/ASTWalker.h"
#include "AST/Decl.h"
#include "AST/Expr.h"
#include <ostream>
#include <string>
#include <memory>

class ASTScopePrinter: public ASTWalker {
private:
  std::ostream& os;

public:
  ASTScopePrinter(std::ostream& stream): os{stream} {};

  bool visitCompilationUnit(std::shared_ptr<CompilationUnit> tree) override {
    os << "---------- " << "unit"<< " ----------" << std::endl;
    for (const std::pair<StringRef, Decl*> pair: tree->getDeclContext()->getDeclMap()) {
      os << pair.first << ": " << (pair.second ? pair.second->getType()->toString() : "<error: no decl> ")<< std::endl;
    }
    return true;
  }

  bool visitFuncDecl(std::shared_ptr<FuncDecl> tree) override {
    os << "---------- " << "func " << tree->getName() << " ----------" << std::endl;
    for (const std::pair<StringRef, Decl*> pair: tree->getDeclContext()->getDeclMap()) {
      os << pair.first << ": " << (pair.second ? pair.second->getType()->toString() : "<error: no decl> ")<< std::endl;
    }
    return true;
  }

  bool visitCompoundStmt(std::shared_ptr<CompoundStmt> tree) override {
    os << "---------- " << "block" << " ----------" << std::endl;
    for (const std::pair<StringRef, Decl*> pair: tree->getDeclContext()->getDeclMap()) {
      os << pair.first << ": " << (pair.second ? pair.second->getType()->toString() : "<error: no decl> ")<< std::endl;
    }
    return true;
  }

  bool visitConditionalStmt(std::shared_ptr<ConditionalStmt> tree) override {
    os << "---------- " << "cond" << " ----------" << std::endl;
    for (const std::pair<StringRef, Decl*> pair: tree->getDeclContext()->getDeclMap()) {
      os << pair.first << ": " << pair.second->getType()->toString() << std::endl;
    }
    return true;
  }
  bool visitWhileLoop(std::shared_ptr<WhileLoop> tree) override {
    os << "---------- " << "loop" << " ----------" << std::endl;
    for (const std::pair<StringRef, Decl*> pair: tree->getDeclContext()->getDeclMap()) {
      os << pair.first << ": " << pair.second->getType()->toString() << std::endl;
    }
    return true;
  }
};

#endif
