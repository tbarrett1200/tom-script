#ifndef AST_STMT_H
#define AST_STMT_H

class Stmt {
public:
  enum class Kind {
    #define STMT(SELF, PARENT) SELF,
    #include "AST/Stmt.def"
    #undef STMT
  };
  virtual Stmt::Kind getKind() const = 0;
};


#endif
