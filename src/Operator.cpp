#include "Operator.h"

OperatorTable::OperatorTable(std::vector<PrecedenceGroup> groups) {
  precedenceGroups = groups;
}

OperatorTable* OperatorTable::globalInstance = new OperatorTable({
  { "Prefix", Associativity::none, Fixity::prefix }
});
