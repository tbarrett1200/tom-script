#include <gtest/gtest.h>
#include "Basic/SourceCode.h"
#include "Basic/CompilerException.h"
#include "Parse/Parser.h"

TEST(ExprParser, parseIntegerExpr) {

  auto _PARSE_RESULT = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseIntegerExpr();
  };

  EXPECT_EQ(_PARSE_RESULT("123")->getText(), "123");
  EXPECT_EQ(_PARSE_RESULT("123")->getInt(), 123);
  EXPECT_ANY_THROW(_PARSE_RESULT("abc"));
  EXPECT_ANY_THROW(_PARSE_RESULT(""));
}

TEST(ExprParser, parseDoubleExpr) {

  auto _PARSE_RESULT = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseDoubleExpr();
  };

  EXPECT_EQ(_PARSE_RESULT("123.456")->getText(), "123.456");
  EXPECT_EQ(_PARSE_RESULT("123.456")->getDouble(), 123.456);
  EXPECT_ANY_THROW(_PARSE_RESULT("abc"));
  EXPECT_ANY_THROW(_PARSE_RESULT(""));

}


TEST(ExprParser, parseValueExpr) {

  auto _PARSE_RESULT = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseValueExpr();
  };

  EXPECT_TRUE(_PARSE_RESULT("123.456")->as<DoubleExpr>() != nullptr);
  EXPECT_TRUE(_PARSE_RESULT("123")->as<IntegerExpr>() != nullptr);
  EXPECT_TRUE(_PARSE_RESULT("abc")->as<IdentifierExpr>() != nullptr);
  EXPECT_ANY_THROW(_PARSE_RESULT(""));

}

TEST(ExprParser, parseExprList) {


  auto _EXPECT_SIZE = [](std::string text, int size) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    auto expr = parser.parseExprList();
    EXPECT_EQ(expr->size(), size);
  };

  auto _EXPECT_THROW = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    EXPECT_THROW(parser.parseExprList(), CompilerException);
  };

  _EXPECT_SIZE("abc: def, 123", 2);
  _EXPECT_SIZE("abc: 1 + 2 + 5, 123 * foo()", 2);
  _EXPECT_SIZE("1,2,3  ,4,5,6", 6);
  _EXPECT_SIZE("foo()()(),12,12", 3);
  _EXPECT_THROW("");
  _EXPECT_THROW("1,2,3,,4");

}

TEST(ExprParser, parseLabeledExprOrExpr) {

  auto _PARSE_RESULT = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseLabeledExprOrExpr();
  };

  auto _EXPECT_THROW = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    EXPECT_THROW(parser.parseLabeledExprOrExpr(), CompilerException);
  };

  EXPECT_TRUE(_PARSE_RESULT("abc: 1")->as<LabeledExpr>() != nullptr);
  EXPECT_TRUE(_PARSE_RESULT("123")->as<IntegerExpr>() != nullptr);
  _EXPECT_THROW("");
}
