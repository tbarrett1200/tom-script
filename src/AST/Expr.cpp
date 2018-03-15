#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/DeclContext.h"
#include "Parse/Parser.h"
#include "AST/AmbiguousType.h"
#include <memory>


AmbiguousType StringExpr::getType(DeclContext* c) const {
  return AmbiguousType({Parser::makeType("String")});
}

AmbiguousType IntegerExpr::getType(DeclContext* c) const {
  return AmbiguousType({Parser::makeType("Int")});

}

AmbiguousType DoubleExpr::getType(DeclContext* c) const {
  return AmbiguousType({Parser::makeType("Double")});
}

AmbiguousType IdentifierExpr::getType(DeclContext* c) const {
  if (!c->hasDecl(getLexeme(), true)) throw std::string("error: use of unresolved identifier '" + getLexeme() + "'\n");
  return c->getDecl(getLexeme(), true).getType();
}

AmbiguousType TupleExpr::getType(DeclContext* c) const {
  return {{make_shared<TupleType>(list->getTypeList(c))}};
}

AmbiguousType OperatorExpr::getType(DeclContext* c) const {
  if (!c->hasDecl(getLexeme(), true)) throw std::string("error: use of unresolved operator '" + getLexeme() + "'\n");
  return c->getDecl(getLexeme(), true).getType();
}

AmbiguousType UnaryExpr::getType(DeclContext* c) const {
  auto op_type = op->getType(c);
  auto expr_type = expr->getType(c);

  if (expr_type.isAmbiguous()) {
    throw std::string("error: ambiguous expression\n");
  } else {
    op_type = op_type.filterFunction(std::make_shared<TypeList>(std::vector<std::shared_ptr<Type>>{expr_type.get()}));
    if (op_type.isAmbiguous()) {
      throw std::string("error: ambiguous operator\n");
    } else if (op_type.isEmpty()) {
      throw std::string("error: operator not defined for expression type\n");
    } else {
      return {{dynamic_pointer_cast<FunctionType>(op_type.get())}};
    }
  }
}

AmbiguousType BinaryExpr::getType(DeclContext* c) const {
  auto op_type = op->getType(c);
  auto left_type = left->getType(c);
  auto right_type = right->getType(c);

  if (left_type.isAmbiguous() || right_type.isAmbiguous()) {
    throw std::string("error: ambiguous expression\n");
  } else {
    auto filtered_op_type = op_type.filterFunction(std::make_shared<TypeList>(std::vector<std::shared_ptr<Type>>{left_type.get(), right_type.get()}));
    if (filtered_op_type.isAmbiguous()) {
      throw std::string("error: ambiguous operator\n");
    } else if (filtered_op_type.isEmpty()) {
      std::stringstream s;
      s << "error: operator is undefined for types (" << left_type.get() << ", " << right_type.get() << ")" << std::endl;
      s << op_type;
      throw std::string(s.str());
    } else {
      std::cout << filtered_op_type.get() << std::endl;
      return {{dynamic_pointer_cast<FunctionType>(filtered_op_type.get())->returns}};
    }
  }
}

ostream& operator<<(ostream& os, Expr* x) {
  if (dynamic_cast<IdentifierExpr*>(x)) {
    auto t = dynamic_cast<IdentifierExpr*>(x);
    os << t->getLexeme();
  } else if (dynamic_cast<FunctionCall*>(x)) {
    auto t = dynamic_cast<FunctionCall*>(x);
    os << t->name << t->arguments;
  } else if (dynamic_cast<TupleExpr*>(x)) {
    auto t = dynamic_cast<TupleExpr*>(x);
    os << "(" << t->list << ")";
  } else {
    os << "expr";
  }

  return os;
}

ostream& operator<<(ostream& os, ExprList* x) {
  if (!x) return os;

  if (x->list) {
    os << x->element << ", " << x->list;
  } else {
    os << x->element;
  }
  return os;
}

ostream& operator<<(ostream& os, ExprLabel* x) {
  os << x->getLexeme();
  return os;
}

AmbiguousType FunctionCall::getType(DeclContext* c) const {
  auto all = name->getType(c);
  std::shared_ptr<TypeList> args = arguments->list ? arguments->list->getTypeList(c) : nullptr;
  auto filtered = all.filterFunction(args);
  if (filtered.isAmbiguous()) {
    std::vector<std::shared_ptr<Type>> types;
    for (auto type: filtered.types) {
      types.push_back(dynamic_pointer_cast<FunctionType>(type)->returns);
    }
    return {types};
  } else if (filtered.isEmpty()) {
    std::stringstream s;
    s << "error: function " << name->getLexeme() << arguments->list->getTypeList(c) << " not declared" << std::endl;
    throw std::string("error: function not found\n");
  } else {
    return {{dynamic_pointer_cast<FunctionType>(filtered.get())->returns}};
  }
}
