#include <gtest/gtest.h>
#include "Basic/SourceCode.h"
#include "Parse/Scope.h"
#include "AST/Type.h"

#include "Basic/CompilerException.h"
#include "Parse/Parser.h"
#include "Parse/Operator.h"


TEST(DeclParser, parseLetDecl) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseLetDecl();
  };

  EXPECT_NO_THROW(parse("let a: Integer = 5"));
}
