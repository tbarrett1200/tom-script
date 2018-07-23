#ifndef AST_XML_PRINT_WALKER
#define AST_XML_PRINT_WALKER

#include "AST/ASTWalker.h"
#include "AST/Decl.h"
#include "AST/Expr.h"
#include <ostream>
#include <string>
#include <memory>

class ASTPrintWalker: public ASTWalker {
private:
  std::ostream& os;
  int indent;

public:
  ASTPrintWalker(std::ostream& stream): os{stream} {};
  void didTraverseNode(std::shared_ptr<TreeElement> m) override {
    --indent;
  }
  void willTraverseNode(std::shared_ptr<TreeElement> m) override {
    std::string format = "";
    for (int i=0; i<indent; i++) {
      format = format + "|  ";
    }
    format = format + "|--";
    os  << format << "[" << typeid(m).name();
    indent++;
    if (std::dynamic_pointer_cast<Decl>(m)) {
      os << " name='" << std::dynamic_pointer_cast<Decl>(m)->getName() << "'";
      auto type = std::dynamic_pointer_cast<Decl>(m)->getType();
      if (type) os << " type='" << type->toString() << "'";
      else  os << " type='" << "<nullptr>" << "'";
    } else if (std::dynamic_pointer_cast<IdentifierExpr>(m)) {
      os << " name='" << std::dynamic_pointer_cast<IdentifierExpr>(m)->getLexeme() << "'";
      auto type = std::dynamic_pointer_cast<IdentifierExpr>(m)->getType();
      if (type) os << " type='" << type->toString() << "'";
      else  os << " type='" << "<nullptr>" << "'";
    } else if (std::dynamic_pointer_cast<Expr>(m)) {
      auto type = std::dynamic_pointer_cast<Expr>(m)->getType();
      if (type) os << " type='" << type->toString() << "'";
      else  os << " type='" << "<nullptr>" << "'";
    }
    os << "]" << std::endl ;
  }
};

#endif
