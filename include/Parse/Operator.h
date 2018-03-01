#ifndef OPERATOR_H
#define OPERATOR_H

#include <string>
#include <vector>

using namespace std;

enum class Associativity {
  left, right, none
};

enum class Fixity {
  prefix, postfix, infix
};

struct PrecedenceGroup {
  std::string name;
  Associativity associativity;
  Fixity fixity;
  bool assignment;
  std::vector<string> operators;
  bool contains(string);
};

class OperatorTable {
private:
  std::vector<PrecedenceGroup> precedenceGroups;
  OperatorTable(std::vector<PrecedenceGroup>);
public:
  static OperatorTable* globalInstance;
  static OperatorTable* getInstance();
  static int size();
  static PrecedenceGroup level(int);
  static Associativity associativity(int precedence);

  std::vector<PrecedenceGroup> getGroups();
};

#endif
