#include <gtest/gtest.h>
#include "Basic/SourceCode.h"
#include "Parse/Scope.h"
#include "AST/Type.h"

#include "Basic/CompilerException.h"
#include "Parse/Parser.h"
#include "Parse/Operator.h"


TEST(DeclParser, parseLetDecl) {

  auto parse = [](std::string text) {
    Parser parser = Parser{text};
    return parser.parseLetDecl();
  };

  std::shared_ptr<LetDecl> letDecl;

  ASSERT_NO_THROW(letDecl = parse("let a: Integer = 5"));
  EXPECT_TRUE(letDecl->getType() != nullptr);
  EXPECT_TRUE(letDecl->getName() == "a");
  ASSERT_TRUE(letDecl->getExpr() != nullptr);
  EXPECT_TRUE(letDecl->getExpr()->getKind() == Expr::Kind::IntegerExpr );

  ASSERT_NO_THROW(letDecl = parse("let b: Double = 5.0"));
  EXPECT_TRUE(letDecl->getType() != nullptr);
  EXPECT_TRUE(letDecl->getName() == "b");
  ASSERT_TRUE(letDecl->getExpr() != nullptr);
  EXPECT_TRUE(letDecl->getExpr()->getKind() == Expr::Kind::DoubleExpr );

  EXPECT_THROW(letDecl = parse("let b: Double"), CompilerException);
  EXPECT_THROW(letDecl = parse("b: Double = 5.0"), CompilerException);
  EXPECT_THROW(letDecl = parse("let b = 5.0"), CompilerException);
  EXPECT_THROW(letDecl = parse("let b 5.0"), CompilerException);
  EXPECT_THROW(letDecl = parse("let b: Double = 5"), CompilerException);

}
