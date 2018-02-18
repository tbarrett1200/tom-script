#include "Operator.h"
#include <algorithm>

OperatorTable::OperatorTable(std::vector<PrecedenceGroup> groups) {
  precedenceGroups = groups;
}

bool Operator::operator==(const Operator& b) {
  return lexeme == b.lexeme;
}

OperatorTable* OperatorTable::globalInstance = new OperatorTable({
  {"Prefix", Associativity::none, Fixity::prefix, false, {
    {"+"},{"-"}, {"!"}
  }},
  {"BitwiseShift", Associativity::none, Fixity::infix, false, {
    {"<<"}, {">>"}
  }},
  {"Multiplication", Associativity::left, Fixity::infix, false, {
    {"*"}, {"/"}, {"%"}
  }},
  {"Addition", Associativity::left, Fixity::infix, false, {
    {"+"}, {"-"}
  }},
  {"Comparative", Associativity::none, Fixity::infix, false, {
    {"=="}, {"!="}, {">"}, {"<"}, {">="}, {"<="}
  }},
  {"LogicalConjunction", Associativity::left, Fixity::infix, false, {
    {"&&"}
  }},
  {"LogicalDisjunction", Associativity::left, Fixity::infix, false, {
    {"&&"}
  }},
  {"Assignment", Associativity::right, Fixity::infix, true, {
    {"="}, {"+="}, {"-="}, {"*="}, {"/="}, {"%="}, {">>="}, {"<<="}
  }}
});

OperatorTable* OperatorTable::getInstance() {
  return OperatorTable::globalInstance;
}

int OperatorTable::size() {
  return OperatorTable::globalInstance->precedenceGroups.size();
}

PrecedenceGroup OperatorTable::level(int p) {
  return OperatorTable::globalInstance->precedenceGroups[p-1];
}

Associativity OperatorTable::associativity(int precedence) {
  return OperatorTable::level(precedence).associativity;
}

bool PrecedenceGroup::contains(Operator o) {
  return std::find(operators.begin(), operators.end(), o) != operators.end();
}

std::vector<PrecedenceGroup> OperatorTable::getGroups() {
  return precedenceGroups;
}
