#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include <memory>
#include <cmath>

#include "AST/DeclarationContext.h"
#include "Parse/Parser.h"

std::shared_ptr<DeclarationContext> globalContext = std::make_shared<DeclarationContext>(std::vector<std::shared_ptr<Decl>>{
  Parser::makeTypeDecl("typedef Int"),
  Parser::makeTypeDecl("typedef Double"),
  Parser::makeTypeDecl("typedef Bool"),
  Parser::makeTypeDecl("typedef String"),
  Parser::makeDecl("typealias Void = ()"),
  Decl::make(Parser::makeFuncDecl("func print(_ :Int) -> Void"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    std::cout << param2 << std::endl;
  }),
  Decl::make(Parser::makeFuncDecl("func print(_:Double) -> Void"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    std::cout << param2 << std::endl;
  }),
  Decl::make(Parser::makeFuncDecl("func sqrt(_:Double) -> Double"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<DoubleExpr>(sqrt(param2)));
  }),
  Decl::make(Parser::makeFuncDecl("func print(_:String) -> Void"), [](RuntimeStack& stack){
    string param2 = stack.top()->as<StringExpr>()->getString();
    std::cout << param2 << std::endl;
  }),
  Decl::make(Parser::makeFuncDecl("func Double(_:Int) -> Double"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<DoubleExpr>(static_cast<double>(param2)));
  }),
  Decl::make(Parser::makeFuncDecl("func Int(_:Double) -> Int"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<IntegerExpr>(static_cast<int>(param2)));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:Int) -> Int"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<IntegerExpr>(+param2));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<IntegerExpr>()->getInt();
    double param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<IntegerExpr>(param2 + param1));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:String, _:String) -> String"), [](RuntimeStack& stack){
    std::string param2 = stack.top()->as<StringExpr>()->getString();
    std::string param1 = stack.top(1)->as<StringExpr>()->getString();
    stack.pushTemp(make_shared<StringExpr>(param2 + param1));
  }),
  Decl::make(Parser::makeFuncDecl("func +(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<DoubleExpr>(param2 + param1));
  }),
  Decl::make(Parser::makeFuncDecl("func -(_:Int) -> Int"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<IntegerExpr>(-param2));
  }),
  Decl::make(Parser::makeFuncDecl("func -(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<IntegerExpr>()->getInt();
    double param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<IntegerExpr>(param1 - param2));
  }),
  Decl::make(Parser::makeFuncDecl("func -(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<DoubleExpr>(param1 - param2));
  }),
  Decl::make(Parser::makeFuncDecl("func *(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<IntegerExpr>()->getInt();
    double param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<IntegerExpr>(param2 * param1));
  }),
  Decl::make(Parser::makeFuncDecl("func *(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<DoubleExpr>(param2 * param1));
  }),
  Decl::make(Parser::makeFuncDecl("func /(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<IntegerExpr>()->getInt();
    double param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<IntegerExpr>(param2 / param1));
  }),
  Decl::make(Parser::makeFuncDecl("func /(_:Double, _:Double) -> Double"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<DoubleExpr>(param2 / param1));
  }),
  Decl::make(Parser::makeFuncDecl("func %(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    int param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<IntegerExpr>(param2 % param1));
  }),
  Decl::make(Parser::makeFuncDecl("func <<(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    int param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<IntegerExpr>(param2 << param1));
  }),
  Decl::make(Parser::makeFuncDecl("func >>(_:Int, _:Int) -> Int"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    int param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<IntegerExpr>(param2 >> param1));
  }),
  Decl::make(Parser::makeFuncDecl("func ==(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    int param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<BoolExpr>(param2 == param1));
  }),
  Decl::make(Parser::makeFuncDecl("func ==(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<BoolExpr>(param2 == param1));
  }),
  Decl::make(Parser::makeFuncDecl("func ==(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param2 = stack.top()->as<BoolExpr>()->getBool();
    bool param1 = stack.top(1)->as<BoolExpr>()->getBool();
    stack.pushTemp(make_shared<BoolExpr>(param2 == param1));
  }),
  Decl::make(Parser::makeFuncDecl("func !=(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    int param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<BoolExpr>(param2 != param1));
  }),
  Decl::make(Parser::makeFuncDecl("func !=(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<BoolExpr>(param2 != param1));
  }),
  Decl::make(Parser::makeFuncDecl("func !=(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param2 = stack.top()->as<BoolExpr>()->getBool();
    bool param1 = stack.top(1)->as<BoolExpr>()->getBool();
    stack.pushTemp(make_shared<BoolExpr>(param2 != param1));
  }),
  Decl::make(Parser::makeFuncDecl("func >(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    int param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<BoolExpr>(param2 > param1));
  }),
  Decl::make(Parser::makeFuncDecl("func >(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<BoolExpr>(param2 > param1));
  }),
  Decl::make(Parser::makeFuncDecl("func <(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    int param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<BoolExpr>(param2 < param1));
  }),
  Decl::make(Parser::makeFuncDecl("func <(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<BoolExpr>(param2 < param1));
  }),
  Decl::make(Parser::makeFuncDecl("func >=(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    int param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<BoolExpr>(param2 >= param1));
  }),
  Decl::make(Parser::makeFuncDecl("func >=(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<BoolExpr>(param2 >= param1));
  }),
  Decl::make(Parser::makeFuncDecl("func <=(_:Int, _:Int) -> Bool"), [](RuntimeStack& stack){
    int param2 = stack.top()->as<IntegerExpr>()->getInt();
    int param1 = stack.top(1)->as<IntegerExpr>()->getInt();
    stack.pushTemp(make_shared<BoolExpr>(param2 <= param1));
  }),
  Decl::make(Parser::makeFuncDecl("func <=(_:Double, _:Double) -> Bool"), [](RuntimeStack& stack){
    double param2 = stack.top()->as<DoubleExpr>()->getDouble();
    double param1 = stack.top(1)->as<DoubleExpr>()->getDouble();
    stack.pushTemp(make_shared<BoolExpr>(param2 <= param1));
  }),
  Decl::make(Parser::makeFuncDecl("func &&(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param2 = stack.top()->as<BoolExpr>()->getBool();
    bool param1 = stack.top(1)->as<BoolExpr>()->getBool();
    stack.pushTemp(make_shared<BoolExpr>(param2 && param1));
  }),
  Decl::make(Parser::makeFuncDecl("func ||(_:Bool, _:Bool) -> Bool"), [](RuntimeStack& stack){
    bool param2 = stack.top()->as<BoolExpr>()->getBool();
    bool param1 = stack.top(1)->as<BoolExpr>()->getBool();
    stack.pushTemp(make_shared<BoolExpr>(param2 || param1));
  }),
  Decl::make(Parser::makeFuncDecl("func !(_:Bool) -> _:Bool"), [](RuntimeStack& stack){
    bool param2 = stack.top()->as<BoolExpr>()->getBool();
    stack.pushTemp(make_shared<BoolExpr>(!param2));
  })
});

#endif
