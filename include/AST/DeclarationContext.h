#ifndef AST_CONTEXT
#define AST_CONTEXT

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <vector>
#include <string>
#include <memory>

class Decl;
class Expr;
class Type;
class TypeList;
class AmbiguousDecl;

/**
 * A container class for scoped declarations. Each DeclarationContext  has its
 * own set of local declarations, as well as a pointer to the parent Declaration
 * context.
 */
class DeclarationContext {
private:
  DeclarationContext* parent = nullptr; /// the parent scope
  std::vector<std::shared_ptr<Decl>> elements = {}; /// the local declarations
public:
  /** constructs a declaration context from a list of declarations */
  DeclarationContext(std::initializer_list<std::shared_ptr<Decl>> e): elements{e} {}
  DeclarationContext(std::vector<std::shared_ptr<Decl>> e): elements{e} {}
  DeclarationContext() = default;

  void setParent(DeclarationContext* p) {
    parent = p;
  }

  DeclarationContext* getParent() {
    return parent;
  }
  /**
   * Adds a declaration to the local scope if not already defined.
   * returns true if added and false if the declaration already exists.
   */
  bool add(std::shared_ptr<Decl> d);

  /** returns true if the given declaration is defined at any scope */
  bool has(std::shared_ptr<Decl> d);

  std::vector<std::shared_ptr<Decl>> local() const { return elements; }

  int getSize();

  std::shared_ptr<TypeList> getFundamentalType(std::shared_ptr<TypeList> t);
  std::shared_ptr<Type> getFundamentalType(std::shared_ptr<Type>);

  /** returns true if the given declaration is defined in the local scope */
  bool hasLocal(std::shared_ptr<Decl> d);

  AmbiguousDecl filter(std::shared_ptr<class IdentifierExpr> e);

  /**
   * Returns an ambiguous declaration set from the lowest scope which has an
   * element matching the given pattern. This allows for lexical shadowing. Although
   * there may be identifical declarations in different scopes, the lowest
   * scope is the only one considered. However, if the pattern does not positively
   * identify potential declarations, then this technique will not work. All
   * Declarations matched by the pattern MUST be viable.
   */
  AmbiguousDecl filter(std::function<bool(std::shared_ptr<Decl>)> func);
};

#endif
