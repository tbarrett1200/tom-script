#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <set>
#include <memory>

#include "AST/Decl.h"
#include "AST/Type.h"

class Member {
public:
  std::string name;
  std::unique_ptr<Type> type;

  Member(const Member&) {

  }
  bool operator==(const Member& m) const {
    return name == m.name && *type == *m.type;
  }
};


class Protocol {
  std::set<Protocol> parents;
  std::set<Member> members;

  bool has(Member m) {
    if (members.find(m) != members.end()) {
      return true;
    } else {
      for (Protocol parent: parents) {
        if (parent.has(m)) return true;
      }
    }
    return false;
  }

  bool conformsTo(Protocol p) const {

  }

};


#endif
