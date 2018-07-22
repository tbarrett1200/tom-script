#ifndef AST_SCOPE_H
#define AST_SCOPE_H

#include <vector>
#include <map>
#include <string>

class Scope {
private:
  std::map<std::string, std::shared_ptr<class Type>> namedTypes;
public:
  bool addType(std::string, std::shared_ptr<class Type>);
  bool hasType(std::string);
  std::shared_ptr<class Type> getType(std::string);
};

class ScopeManager {
private:
  std::vector<Scope> scopes;

public:
  ScopeManager();
  void push();
  void pop();
  bool addType(std::string, std::shared_ptr<Type>);
  bool hasType(std::string);
  std::shared_ptr<Type> getType(std::string);
};


#endif
