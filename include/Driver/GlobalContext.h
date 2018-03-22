#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include "AST/DeclarationContext.h"
#include "Parse/Parser.h"

DeclarationContext globalContext {
  Parser::makeTypeDecl("typedef Int"),
  Parser::makeTypeDecl("typedef Double"),
  Parser::makeTypeDecl("typedef Bool"),
  Parser::makeTypeDecl("typedef String"),
  Parser::makeDecl("typealias Void = ()"),
  Decl::make(Parser::makeFuncDecl("func Print(Int) -> Void"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    std::cout << param1 << std::endl;
    return nullptr;
  }),
  Decl::make(Parser::makeFuncDecl("func Print(Double) -> Void"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    std::cout << param1 << std::endl;
    return nullptr;
  }),
  Decl::make(Parser::makeFuncDecl("func Print(String) -> Void"), [](std::shared_ptr<TupleExpr> args){
    string param1 = (*args)[0]->as<StringExpr>()->getString();
    std::cout << param1.substr(1, param1.size()-1) << std::endl;
    return nullptr;
  }),
  Decl::make(Parser::makeFuncDecl("func Double(Int) -> Double"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    return make_shared<DoubleExpr>(static_cast<double>(param1));
  }),
  Decl::make(Parser::makeFuncDecl("func Int(Double) -> Int"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    return make_shared<IntegerExpr>(static_cast<int>(param1));
  }),
  Decl::make(Parser::makeFuncDecl("func +(Int) -> Int"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    return make_shared<IntegerExpr>(+param1);
  }),
  Decl::make(Parser::makeFuncDecl("func +(Int, Int) -> Int"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    double param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<IntegerExpr>(param1 + param2);
  }),
  Decl::make(Parser::makeFuncDecl("func +(String, String) -> String"), [](std::shared_ptr<TupleExpr> args){
    std::string param1 = (*args)[0]->as<StringExpr>()->getString();
    std::string param2 = (*args)[1]->as<StringExpr>()->getString();
    return make_shared<StringExpr>(param1 + param2);
  }),
  Decl::make(Parser::makeFuncDecl("func +(Double, Double) -> Double"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<DoubleExpr>(param1 + param2);
  }),
  Decl::make(Parser::makeFuncDecl("func -(Int) -> Int"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    return make_shared<IntegerExpr>(-param1);
  }),
  Decl::make(Parser::makeFuncDecl("func -(Int, Int) -> Int"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    double param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<IntegerExpr>(param1 - param2);
  }),
  Decl::make(Parser::makeFuncDecl("func -(Double, Double) -> Double"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<DoubleExpr>(param1 - param2);
  }),
  Decl::make(Parser::makeFuncDecl("func *(Int, Int) -> Int"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    double param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<IntegerExpr>(param1 * param2);
  }),
  Decl::make(Parser::makeFuncDecl("func *(Double, Double) -> Double"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<DoubleExpr>(param1 * param2);
  }),
  Decl::make(Parser::makeFuncDecl("func /(Int, Int) -> Int"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    double param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<IntegerExpr>(param1 / param2);
  }),
  Decl::make(Parser::makeFuncDecl("func /(Double, Double) -> Double"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<DoubleExpr>(param1 / param2);
  }),
  Decl::make(Parser::makeFuncDecl("func %(Int, Int) -> Int"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    int param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<IntegerExpr>(param1 % param2);
  }),
  Decl::make(Parser::makeFuncDecl("func <<(Int, Int) -> Int"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    int param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<IntegerExpr>(param1 << param2);
  }),
  Decl::make(Parser::makeFuncDecl("func >>(Int, Int) -> Int"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    int param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<IntegerExpr>(param1 >> param2);
  }),
  Decl::make(Parser::makeFuncDecl("func ==(Int, Int) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    int param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<BoolExpr>(param1 == param2);
  }),
  Decl::make(Parser::makeFuncDecl("func ==(Double, Double) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<BoolExpr>(param1 == param2);
  }),
  Decl::make(Parser::makeFuncDecl("func ==(Bool, Bool) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    bool param1 = (*args)[0]->as<BoolExpr>()->getBool();
    bool param2 = (*args)[1]->as<BoolExpr>()->getBool();
    return make_shared<BoolExpr>(param1 == param2);
  }),
  Decl::make(Parser::makeFuncDecl("func !=(Int, Int) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    int param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<BoolExpr>(param1 != param2);
  }),
  Decl::make(Parser::makeFuncDecl("func !=(Double, Double) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<BoolExpr>(param1 != param2);
  }),
  Decl::make(Parser::makeFuncDecl("func !=(Bool, Bool) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    bool param1 = (*args)[0]->as<BoolExpr>()->getBool();
    bool param2 = (*args)[1]->as<BoolExpr>()->getBool();
    return make_shared<BoolExpr>(param1 != param2);
  }),
  Decl::make(Parser::makeFuncDecl("func >(Int, Int) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    int param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<BoolExpr>(param1 > param2);
  }),
  Decl::make(Parser::makeFuncDecl("func >(Double, Double) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<BoolExpr>(param1 > param2);
  }),
  Decl::make(Parser::makeFuncDecl("func <(Int, Int) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    int param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<BoolExpr>(param1 < param2);
  }),
  Decl::make(Parser::makeFuncDecl("func <(Double, Double) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<BoolExpr>(param1 < param2);
  }),
  Decl::make(Parser::makeFuncDecl("func >=(Int, Int) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    int param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<BoolExpr>(param1 >= param2);
  }),
  Decl::make(Parser::makeFuncDecl("func >=(Double, Double) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<BoolExpr>(param1 >= param2);
  }),
  Decl::make(Parser::makeFuncDecl("func <=(Int, Int) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    int param1 = (*args)[0]->as<IntegerExpr>()->getInt();
    int param2 = (*args)[1]->as<IntegerExpr>()->getInt();
    return make_shared<BoolExpr>(param1 <= param2);
  }),
  Decl::make(Parser::makeFuncDecl("func <=(Double, Double) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    double param1 = (*args)[0]->as<DoubleExpr>()->getDouble();
    double param2 = (*args)[1]->as<DoubleExpr>()->getDouble();
    return make_shared<BoolExpr>(param1 <= param2);
  }),
  Decl::make(Parser::makeFuncDecl("func &&(Bool, Bool) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    bool param1 = (*args)[0]->as<BoolExpr>()->getBool();
    bool param2 = (*args)[1]->as<BoolExpr>()->getBool();
    return make_shared<BoolExpr>(param1 && param2);
  }),
  Decl::make(Parser::makeFuncDecl("func ||(Bool, Bool) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    bool param1 = (*args)[0]->as<BoolExpr>()->getBool();
    bool param2 = (*args)[1]->as<BoolExpr>()->getBool();
    return make_shared<BoolExpr>(param1 || param2);
  }),
  Decl::make(Parser::makeFuncDecl("func !(Bool) -> Bool"), [](std::shared_ptr<TupleExpr> args){
    bool param1 = (*args)[0]->as<BoolExpr>()->getBool();
    return make_shared<BoolExpr>(!param1);
  })
};

#endif
