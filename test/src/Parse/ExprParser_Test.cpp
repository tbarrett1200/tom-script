#include <gtest/gtest.h>
#include "Basic/SourceCode.h"
#include "Parse/Scope.h"
#include "AST/Type.h"
#include "Basic/CompilerException.h"
#include "Parse/Parser.h"
#include "Parse/Operator.h"


TEST(ExprParser, parseIntegerExpr) {

  auto _PARSE_RESULT = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseIntegerExpr();
  };

  EXPECT_EQ(_PARSE_RESULT("123")->getText(), "123");
  EXPECT_EQ(_PARSE_RESULT("123")->getInt(), 123);
  EXPECT_THROW(_PARSE_RESULT("abc"), CompilerException);
  EXPECT_THROW(_PARSE_RESULT(""), CompilerException);
}

TEST(ExprParser, parseIdentifierExpr) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    parser.getScopeManager().addType("abc", IntegerType::getInstance());
    return parser.parseIdentifier();
  };

  ScopeManager m;
  EXPECT_THROW(parse("def"), CompilerException);
  m.addType("abc", IntegerType::getInstance());
  EXPECT_NO_THROW(parse("abc"));
  EXPECT_TRUE(parse("abc")->getType()->isIntegerType());
}


TEST(ExprParser, parseDoubleExpr) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseDoubleExpr();
  };

  EXPECT_EQ(parse("123.456")->getText(), "123.456");
  EXPECT_EQ(parse("123.456")->getDouble(), 123.456);
  EXPECT_THROW(parse("abc"), CompilerException);
  EXPECT_THROW(parse(""), CompilerException);

}


TEST(ExprParser, parseValueExpr) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    parser.getScopeManager().addType("abc", IntegerType::getInstance());
    return parser.parseValueExpr();
  };

  EXPECT_TRUE(parse("123.456")->as<DoubleExpr>() != nullptr);
  EXPECT_TRUE(parse("123")->as<IntegerExpr>() != nullptr);
  EXPECT_ANY_THROW(parse("def"));
  EXPECT_TRUE(parse("abc")->as<IdentifierExpr>() != nullptr);
  EXPECT_ANY_THROW(parse(""));

}

TEST(ExprParser, parseUnaryExpr) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    parser.getScopeManager().addType("abc", IntegerType::getInstance());
    return parser.parseUnaryExpr();
  };

  EXPECT_NO_THROW(parse("+1"));
  EXPECT_NO_THROW(parse("-1"));
  EXPECT_NO_THROW(parse("+1.0"));
  EXPECT_NO_THROW(parse("-5.0"));

}

TEST(ExprParser, parseBinaryExpr) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    parser.getScopeManager().addType("abc", IntegerType::getInstance());
    return parser.parseBinaryExpr(OperatorTable::size());
  };

  EXPECT_NO_THROW(parse("1+1"));
  EXPECT_NO_THROW(parse("4-abc"));
  EXPECT_THROW(parse("4-def"), CompilerException);

  EXPECT_NO_THROW(parse("5.0+1.0"));
  EXPECT_NO_THROW(parse("3.0-5.0*8.0"));

}

TEST(ExprParser, parseExprList) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    parser.getScopeManager().addType("abc", IntegerType::getInstance());
    return parser.parseExprList();
  };

  EXPECT_EQ(parse("1,2,3  ,4,5,6").size(), 6);
  EXPECT_THROW(parse(""), CompilerException);
  EXPECT_THROW(parse("1,2,3,,4"), CompilerException);

}
