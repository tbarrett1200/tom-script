#ifndef TYPE_ANNOTATABLE_H
#define TYPE_ANNOTATABLE_H

#include "AST/Type.h"

class TypeAnnotatable {
public:
  TypeAnnotatable() {}
  virtual ~TypeAnnotatable() {}
  virtual void setType(std::shared_ptr<Type>) = 0;
  virtual std::shared_ptr<Type> getType() const = 0;
  virtual bool isTypeSet() const = 0;
};

#endif
