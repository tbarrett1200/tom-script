#include <gtest/gtest.h>
#include "Basic/SourceCode.h"
#include "Parse/Scope.h"
#include "AST/Type.h"

#include "Basic/CompilerException.h"
#include "Parse/Parser.h"
#include "Parse/Operator.h"



TEST(StmtParser, parseDeclStmt) {

  auto parse = [](std::string text) {
    Parser parser = Parser{text};
    return parser.parseDeclStmt();
  };

  EXPECT_NO_THROW(parse("let a: Integer = 5\n"));
}

TEST(StmtParser, parseStmtList) {

  auto parse = [](std::string text) {
    Parser parser = Parser{text};
    return parser.parseStmtList();
  };

  EXPECT_NO_THROW(parse("let a: Integer = 5\nreturn a\n"));
}

TEST(StmtParser, parseCompoundStmt) {

  auto parse = [](std::string text) {
    Parser parser = Parser{text};
    return parser.parseCompoundStmt();
  };

  EXPECT_NO_THROW(parse("{\nlet a: Integer = 5\nreturn a\n}"));
  EXPECT_NO_THROW(parse("{\nlet a: Integer = 5\nreturn a\n}\n"));
  EXPECT_NO_THROW(parse("{\nlet a: Integer = 5\n\nreturn a\n}\n"));
  EXPECT_NO_THROW(parse("\n{\nlet a: Integer = 5\nreturn a\n}\n"));

}

TEST(StmtParser, parseReturnStmt) {
  auto parse = [](std::string text) {
    Parser parser = Parser{text};
    return parser.parseReturnStmt();
  };

  EXPECT_NO_THROW(parse("return 5\n"));
  EXPECT_ANY_THROW(parse("return 5"));
  EXPECT_NO_THROW(parse("return\n"));
  EXPECT_ANY_THROW(parse("return"));
}