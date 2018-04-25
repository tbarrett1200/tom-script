#include "AST/DeclarationContext.h"
#include "AST/AmbiguousDecl.h"
#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Type.h"
#include "Parse/Parser.h"
#include "Sema/RuntimeStack.h"
#include <cmath>
std::shared_ptr<DeclarationContext> DeclarationContext::globalContext = std::make_shared<DeclarationContext>(std::vector<std::shared_ptr<Decl>>{
  Parser::makeTypeDecl("typedef Int"),
  Parser::makeTypeDecl("typedef Double"),
  Parser::makeTypeDecl("typedef Bool"),
  Parser::makeTypeDecl("typedef String"),
  Parser::makeDecl("typealias Void = ()"),
  Decl::make(Parser::makeFuncDecl("func stack() -> Void"), [](RuntimeStack& stack){
    for(int i = 0; i < stack.size()-1; i++) {
      std::cout << "[" << i << "]\t" << stack.at(i) << std::endl;
    }
  }),
  Decl::make(Parser::makeFuncDecl("func print(_ :Int) -> Void"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    std::cout << param1 << std::endl;
  }),
  Decl::make(Parser::makeFuncDecl("func print(_:Double) -> Void"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    std::cout << param1 << std::endl;
  }),
  Decl::make(Parser::makeFuncDecl("func sqrt(_:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(sqrt(param1)));
  }),
  Decl::make(Parser::makeFuncDecl("func print(_:String) -> Void"), [](RuntimeStack& stack){
    string param1 = stack.param(1)->as<StringExpr>()->getString();
    std::cout << param1 << std::endl;
  }),
  Decl::make(Parser::makeFuncDecl("func Double(_:Int) -> Double"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<DoubleExpr>(static_cast<double>(param1)));
  }),
  Decl::make(Parser::makeFuncDecl("func Int(_:Double) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<IntegerExpr>(static_cast<int>(param1)));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(+param1));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    double param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 + param2));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:String, _:String) -> String"), [](RuntimeStack& stack){
    std::string param1 = stack.param(1)->as<StringExpr>()->getString();
    std::string param2 = stack.param(2)->as<StringExpr>()->getString();
    stack.setResult(make_shared<StringExpr>(param1 + param2));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(param1 + param2));
  }),
  Decl::make(Parser::makeFuncDecl("func -(_:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(-param1));
  }),
  Decl::make(Parser::makeFuncDecl("func -(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    double param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 - param2));
  }),
  Decl::make(Parser::makeFuncDecl("func -(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(param1 - param2));
  }),
  Decl::make(Parser::makeFuncDecl("func *(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    double param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 * param2));
  }),
  Decl::make(Parser::makeFuncDecl("func *(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(param1 * param2));
  }),
  Decl::make(Parser::makeFuncDecl("func /(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    double param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 / param2));
  }),
  Decl::make(Parser::makeFuncDecl("func /(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(param1 / param2));
  }),
  Decl::make(Parser::makeFuncDecl("func %(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    int param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 % param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <<(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    int param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 << param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >>(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    int param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 >> param2));
  }),
  Decl::make(Parser::makeFuncDecl("func ==(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    int param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 == param2));
  }),
  Decl::make(Parser::makeFuncDecl("func ==(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 == param2));
  }),
  Decl::make(Parser::makeFuncDecl("func ==(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param1 = stack.param(1)->as<BoolExpr>()->getBool();
    bool param2 = stack.param(2)->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(param1 == param2));
  }),
  Decl::make(Parser::makeFuncDecl("func !=(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    int param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 != param2));
  }),
  Decl::make(Parser::makeFuncDecl("func !=(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 != param2));
  }),
  Decl::make(Parser::makeFuncDecl("func !=(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param1 = stack.param(1)->as<BoolExpr>()->getBool();
    bool param2 = stack.param(2)->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(param1 != param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    int param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 > param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 > param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    int param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 < param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 < param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >=(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    int param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 >= param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >=(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 >= param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <=(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.param(1)->as<IntegerExpr>()->getInt();
    int param2 = stack.param(2)->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 <= param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <=(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.param(1)->as<DoubleExpr>()->getDouble();
    double param2 = stack.param(2)->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 <= param2));
  }),
  Decl::make(Parser::makeFuncDecl("func &&(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param1 = stack.param(1)->as<BoolExpr>()->getBool();
    bool param2 = stack.param(2)->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(param1 && param2));
  }),
  Decl::make(Parser::makeFuncDecl("func ||(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param1 = stack.param(1)->as<BoolExpr>()->getBool();
    bool param2 = stack.param(2)->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(param1 || param2));
  }),
  Decl::make(Parser::makeFuncDecl("func !(_:Bool) -> _:Bool"), [](RuntimeStack& stack){
    bool param1 = stack.param(1)->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(!param1));
  })
});

bool DeclarationContext::add(std::shared_ptr<Decl> d) {
  for (auto element: elements) {
    if (element->getName() == d->getName() && equal(element->getType(), d->getType(), this)) {
      return false;
    }
  }
  d->location = std::make_shared<StackReference>(getSize()+1);
  elements.push_back(d);
  return true;
}

bool DeclarationContext::add(std::shared_ptr<Decl> d, std::function<int(const DeclarationContext&)> loc) {
  for (auto element: elements) {
    if (element->getName() == d->getName() && equal(element->getType(), d->getType(), this)) {
      return false;
    }
  }
  d->location = std::make_shared<StackReference>(loc(*this));
  elements.push_back(d);
  return true;
}


bool DeclarationContext::hasLocal(std::shared_ptr<Decl> d) {
  if (!d) return false;

  for (auto element: elements) {
    if (element->getName() == d->getName()) {
      return true;
    }
  }
  return false;
}

std::shared_ptr<TypeList> DeclarationContext::getFundamentalType(std::shared_ptr<TypeList> t) {
  if (!t) return nullptr;
  auto e = getFundamentalType(t->element);
  auto list = t->list ? getFundamentalType(t->list) : nullptr;
  return std::make_shared<TypeList>(e, list);
}

std::shared_ptr<Type> DeclarationContext::getFundamentalType(std::shared_ptr<Type> t) {
  if (!t) return nullptr;
  if (t->getKind() == Type::Kind::TupleType) {
    return std::make_shared<TupleType>(getFundamentalType(t->as<TupleType>()->list));
  } else if (t->getKind() == Type::Kind::FunctionType) {
      return std::make_shared<FunctionType>(getFundamentalType(t->as<FunctionType>()->params), getFundamentalType(t->as<FunctionType>()->returns));
  } else if (t->getKind() == Type::Kind::LabeledType) {
      return std::make_shared<LabeledType>(t->as<LabeledType>()->label, getFundamentalType(t->as<LabeledType>()->type));
  } else if (t->getKind() == Type::Kind::ListType) {
      return std::make_shared<ListType>(getFundamentalType(t->as<ListType>()->type));
  } else if (t->getKind() == Type::Kind::MapType) {
      return std::make_shared<MapType>(getFundamentalType(t->as<MapType>()->keyType), getFundamentalType(t->as<MapType>()->valType));
  } else {
    return t;
  }
}

int DeclarationContext::getSize() const {
  int size = 0;
  for (auto decl: elements) {
    if (std::dynamic_pointer_cast<VarDecl>(decl) || std::dynamic_pointer_cast<LetDecl>(decl))
      size++;
  }
  return size;
}

bool DeclarationContext::has(std::shared_ptr<Decl> d) {
  return hasLocal(d) || (parent && parent->has(d));
}

AmbiguousDecl DeclarationContext::filter(std::shared_ptr<class IdentifierExpr> e) {
  AmbiguousDecl self = AmbiguousDecl{elements}.filter(e);
  if (self.isEmpty()) {
    return parent ? parent->filter(e) : self;
  } else return self;
}

AmbiguousDecl DeclarationContext::filter(std::function<bool(std::shared_ptr<Decl>)> func) {
  AmbiguousDecl self = AmbiguousDecl{elements}.filter(func);
  if (self.isEmpty()) {
    return parent ? parent->filter(func) : self;
  } else return self;
}
