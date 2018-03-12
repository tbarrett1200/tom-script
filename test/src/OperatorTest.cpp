#include <gtest/gtest.h>
#include "Parse/Operator.h"

/*
TEST(Operator, globalInstance) {
  OperatorTable* singleton = OperatorTable::getInstance();
  EXPECT_NE(singleton, nullptr);
  std::vector<PrecedenceGroup> groups = singleton->getGroups();
  EXPECT_EQ(groups.size(),8);
  PrecedenceGroup group1 = groups[0];
  EXPECT_EQ(group1.name, "Prefix");
  EXPECT_EQ(group1.associativity, Associativity::none);
  EXPECT_EQ(group1.fixity, Fixity::prefix);
  EXPECT_FALSE(group1.assignment);
  std::vector<Operator> operators = group1.operators;
  EXPECT_EQ(operators.size(),3);
}
*/
