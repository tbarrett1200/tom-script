#ifndef AST_XML_PRINT_WALKER
#define AST_XML_PRINT_WALKER

#include "AST/ASTWalker.h"
#include "AST/Decl.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"
#include <ostream>
#include <string>
#include <memory>

class ASTPrintWalker: public ASTWalker {
private:
  std::ostream &os_;
public:
  ASTPrintWalker(std::ostream &os): os_{os} {}
  void didTraverseNode(TreeElement& m) override {
    if (m.getChildren().size() > 0) {
      os_.seekp((int)os_.tellp()-1);
    }
    os_ << "]},";
  };
  void willTraverseNode(TreeElement& m) override {
    os_ << "{ \"name\": \"" << m.name() << "\", \"children\": [";
  };
};

#endif
