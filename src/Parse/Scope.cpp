#include "Parse/Scope.h"

bool Scope::addType(std::string name, std::shared_ptr<class Type> type) {
  return namedTypes.emplace(name, type).second;
}

bool Scope::hasType(std::string name) {
  return namedTypes.find(name) != namedTypes.end();
};

std::shared_ptr<class Type> Scope::getType(std::string name) {
  return namedTypes.at(name);
}

ScopeManager::ScopeManager() {
  scopes.push_back(Scope());
}
void ScopeManager::push() {
  scopes.push_back(Scope());
}
void ScopeManager::pop() {
  scopes.pop_back();
}
bool ScopeManager::addType(std::string name, std::shared_ptr<Type> type) {
  return scopes.back().addType(name, type);
}
bool ScopeManager::hasType(std::string name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    if (it->hasType(name)) {
      return true;
    }
  }
  return false;
}
std::shared_ptr<Type> ScopeManager::getType(std::string name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    if (it->hasType(name)) {
      return it->getType(name);
    }
  }
  return nullptr;
}
