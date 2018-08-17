#include <gtest/gtest.h>
#include "Basic/SourceCode.h"
#include "AST/Type.h"

#include "Basic/CompilerException.h"
#include "Parse/Parser.h"
#include "Parse/Operator.h"


TEST(DeclParser, parseLetDecl) {

  auto parse = [](std::string text) {
    std::stringstream ss{text};
    std::shared_ptr<SourceFile> src = std::make_shared<SourceFile>(ss);
    Parser parser = Parser{src};
    return parser.parseLetDecl();
  };

  std::unique_ptr<LetDecl> letDecl;

  ASSERT_NO_THROW(letDecl = parse("let a: Int = 5"));
  EXPECT_EQ(letDecl->getType(), IntegerType::getInstance());
  EXPECT_EQ(letDecl->getName(), StringRef{"a"});
  EXPECT_EQ(letDecl->getExpr().getKind(), Expr::Kind::IntegerExpr );

  ASSERT_NO_THROW(letDecl = parse("let b: Double = 5.0"));
  EXPECT_EQ(letDecl->getType(), DoubleType::getInstance());
  EXPECT_EQ(letDecl->getName(), StringRef{"b"});
  EXPECT_EQ(letDecl->getExpr().getKind(), Expr::Kind::DoubleExpr );

}
