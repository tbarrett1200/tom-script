#ifndef AST_CONTEXT
#define AST_CONTEXT

#include "AST/Decl.h"

#include <string>

using namespace std;

class DeclContext {
private:
  DeclContext *parent;
public:
  DeclContext* getParent();
  void addDecl(Decl*);
  void removeDecl();
  bool hasDecl();
  Decl* getDecl(string);
};

#endif
