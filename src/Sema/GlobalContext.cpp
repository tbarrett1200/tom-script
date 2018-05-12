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
      std::cout << "[" << i << "]\t" << stack.get(i) << std::endl;
    }
  }),
  Decl::make(Parser::makeFuncDecl("func print(_ :Int) -> Void"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    std::cout << param1 << std::endl;
  }),
  Decl::make(Parser::makeFuncDecl("func print(_:Double) -> Void"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    std::cout << param1 << std::endl;
  }),
  Decl::make(Parser::makeFuncDecl("func sqrt(_:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(sqrt(param1)));
  }),
  Decl::make(Parser::makeFuncDecl("func print(_:String) -> Void"), [](RuntimeStack& stack){
    string param1 = stack.get(ComputedAddress::param(1))->as<StringExpr>()->getString();
    std::cout << param1 << std::endl;
  }),
  Decl::make(Parser::makeFuncDecl("func Double(_:Int) -> Double"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<DoubleExpr>(static_cast<double>(param1)));
  }),
  Decl::make(Parser::makeFuncDecl("func Int(_:Double) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<IntegerExpr>(static_cast<int>(param1)));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(+param1));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    double param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 + param2));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:String, _:String) -> String"), [](RuntimeStack& stack){
    std::string param1 = stack.get(ComputedAddress::param(1))->as<StringExpr>()->getString();
    std::string param2 = stack.get(ComputedAddress::param(2))->as<StringExpr>()->getString();
    stack.setResult(make_shared<StringExpr>(param1 + param2));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(param1 + param2));
  }),
  Decl::make(Parser::makeFuncDecl("func -(_:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(-param1));
  }),
  Decl::make(Parser::makeFuncDecl("func -(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    double param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 - param2));
  }),
  Decl::make(Parser::makeFuncDecl("func -(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(param1 - param2));
  }),
  Decl::make(Parser::makeFuncDecl("func *(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    double param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 * param2));
  }),
  Decl::make(Parser::makeFuncDecl("func *(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(param1 * param2));
  }),
  Decl::make(Parser::makeFuncDecl("func /(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    double param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 / param2));
  }),
  Decl::make(Parser::makeFuncDecl("func /(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<DoubleExpr>(param1 / param2));
  }),
  Decl::make(Parser::makeFuncDecl("func %(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    int param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 % param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <<(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    int param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 << param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >>(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    int param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<IntegerExpr>(param1 >> param2));
  }),
  Decl::make(Parser::makeFuncDecl("func ==(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    int param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 == param2));
  }),
  Decl::make(Parser::makeFuncDecl("func ==(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 == param2));
  }),
  Decl::make(Parser::makeFuncDecl("func ==(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param1 = stack.get(ComputedAddress::param(1))->as<BoolExpr>()->getBool();
    bool param2 = stack.get(ComputedAddress::param(2))->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(param1 == param2));
  }),
  Decl::make(Parser::makeFuncDecl("func !=(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    int param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 != param2));
  }),
  Decl::make(Parser::makeFuncDecl("func !=(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 != param2));
  }),
  Decl::make(Parser::makeFuncDecl("func !=(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param1 = stack.get(ComputedAddress::param(1))->as<BoolExpr>()->getBool();
    bool param2 = stack.get(ComputedAddress::param(2))->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(param1 != param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    int param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 > param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 > param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    int param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 < param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 < param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >=(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    int param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 >= param2));
  }),
  Decl::make(Parser::makeFuncDecl("func >=(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 >= param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <=(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param1 = stack.get(ComputedAddress::param(1))->as<IntegerExpr>()->getInt();
    int param2 = stack.get(ComputedAddress::param(2))->as<IntegerExpr>()->getInt();
    stack.setResult(make_shared<BoolExpr>(param1 <= param2));
  }),
  Decl::make(Parser::makeFuncDecl("func <=(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param1 = stack.get(ComputedAddress::param(1))->as<DoubleExpr>()->getDouble();
    double param2 = stack.get(ComputedAddress::param(2))->as<DoubleExpr>()->getDouble();
    stack.setResult(make_shared<BoolExpr>(param1 <= param2));
  }),
  Decl::make(Parser::makeFuncDecl("func &&(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param1 = stack.get(ComputedAddress::param(1))->as<BoolExpr>()->getBool();
    bool param2 = stack.get(ComputedAddress::param(2))->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(param1 && param2));
  }),
  Decl::make(Parser::makeFuncDecl("func ||(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param1 = stack.get(ComputedAddress::param(1))->as<BoolExpr>()->getBool();
    bool param2 = stack.get(ComputedAddress::param(2))->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(param1 || param2));
  }),
  Decl::make(Parser::makeFuncDecl("func !(_:Bool) -> _:Bool"), [](RuntimeStack& stack){
    bool param1 = stack.get(ComputedAddress::param(1))->as<BoolExpr>()->getBool();
    stack.setResult(make_shared<BoolExpr>(!param1));
  }),
});
