#ifndef OPERATOR_H
#define OPERATOR_H

#include <string>
#include <vector>

enum class Associativity {
  left, right, none
};

enum class Fixity {
  prefix, postfix, infix
};

struct Operator {
  std::string lexeme;
};

struct PrecedenceGroup {
  std::string name;
  Associativity associativity;
  Fixity fixity;
  bool assignment;
  std::vector<Operator> operators;
};

class OperatorTable {
private:
  std::vector<PrecedenceGroup> precedenceGroups;
  OperatorTable(std::vector<PrecedenceGroup>);
public:
  static OperatorTable* globalInstance;
  static OperatorTable* getInstance();
};

#endif
