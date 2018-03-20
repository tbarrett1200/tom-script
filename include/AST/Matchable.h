#ifndef TREE_H
#define TREE_H

#include <vector>
#include <algorithm>
#include <initializer_list>
#include <string>

class Matchable {
public:
  virtual bool matches(const Matchable& m, bool exact = true) const = 0;
  virtual bool isTerminal() const = 0;
  virtual bool isNonTerminal() const = 0;
  virtual std::vector<std::string> getFlattenedTokens() const = 0;
};

class Terminal : virtual public Matchable {
public:
  virtual std::string getLexeme() const = 0;
  virtual bool isTerminal() const { return true; }
  virtual bool isNonTerminal() const { return false; }
  bool matches(const Matchable& m, bool exact = true) const {
    if (m.isTerminal()) {
      return getLexeme() == dynamic_cast<const Terminal&>(m).getLexeme();
    } else return false;
  }
  std::vector<std::string> getFlattenedTokens() const {
    return {this->getLexeme()};
  }
};

class NonTerminal : virtual public Matchable  {
public:
  virtual std::vector<std::shared_ptr<Matchable>> getChildren() const = 0;
  virtual bool isTerminal() const { return false; }
  virtual bool isNonTerminal() const { return true; }
  std::vector<std::string> getFlattenedTokens() const {
    std::vector<std::string> list;
    for (auto child: getChildren()) {
      auto tokens = child->getFlattenedTokens();
      list.insert(list.end(), tokens.begin(), tokens.end());
    }
    return list;
  }
  bool matches(const Matchable& m, bool exact = true) const {
    if (m.isNonTerminal()) {
      auto mine = getChildren();
      auto other = dynamic_cast<const NonTerminal&>(m).getChildren();
      if ((!exact && mine.size() >= other.size()) || (exact && mine.size() == other.size())) {
        for (int i=0; i<other.size(); i++) {
          if (!(mine[i]->matches(*other[i], exact))) return false;
        }
        return true;
      } else return false;
    } else return false;
  }
};



#endif
