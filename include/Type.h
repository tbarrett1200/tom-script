#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <exception>

using std::string;

class Type {
public:
  virtual int size() = 0;
};

class Builtin : public Type {
public:
  enum class Type {
    int64, float64
  };
  Builtin(Builtin::Type);
  Builtin(string);
  int size();
  Builtin::Type type;
};

#endif
