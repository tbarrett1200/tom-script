#include "Parse/Parser.h"
#include "AST/Stmt.h"

#include <memory>


std::unique_ptr<Stmt> Parser::parseStmt()  {
  switch(token_.type()) {
    case Token::l_brace: return parseCompoundStmt();
    case Token::kw_if: return parseConditionalBlock();
    case Token::kw_return: return parseReturnStmt();
    case Token::kw_while: return parseWhileLoop();
    case Token::kw_var:
    case Token::kw_let:
    case Token::kw_func:
    case Token::kw_extern:
    case Token::kw_struct:
    case Token::kw_typealias: return parseDeclStmt();
    case Token::identifier:
    case Token::integer_literal:
    case Token::double_literal:
    case Token::operator_id:
    case Token::string_literal:
    case Token::l_paren: return parseExprStmt();
    default:
      std::stringstream ss;
      ss <<  "expected statement but found " << token_.lexeme();
    throw CompilerException(token_.location(), ss.str());
  }
}

std::unique_ptr<CompoundStmt> Parser::parseCompoundStmt()  {
  expectToken(Token::l_brace, "left brace");
  while(token_.is(Token::new_line)) consume();
  if (consumeToken(Token::r_brace)) return std::make_unique<CompoundStmt>(std::vector<std::unique_ptr<Stmt>>());
  auto list = parseStmtList();
  while(token_.is(Token::new_line)) consume();
  expectToken(Token::r_brace, "right brace");
  return std::make_unique<CompoundStmt>(std::move(list));
}

std::unique_ptr<ConditionalStmt> Parser::parseConditionalStmt() {
  if (token_.is(Token::kw_let)) {
    auto let_decl = parseLetDecl();
    auto stmt = parseCompoundStmt();
    return std::make_unique<ConditionalStmt>(std::move(let_decl), std::move(stmt));
  } else {
    auto expr = parseExpr();
    if (consumeToken(Token::kw_then)) {
      std::vector<std::unique_ptr<Stmt>> stmts;
      stmts.push_back(parseStmt());
      std::unique_ptr<CompoundStmt> stmt = std::make_unique<CompoundStmt>(std::move(stmts));
      return std::make_unique<ConditionalStmt>(std::move(expr), std::move(stmt));
    } else {
      std::unique_ptr<CompoundStmt> stmt = parseCompoundStmt();
      return std::make_unique<ConditionalStmt>(std::move(expr), std::move(stmt));
    }
  }
}

std::unique_ptr<ConditionalBlock> Parser::parseConditionalBlock()  {
  std::vector<std::unique_ptr<Stmt>> stmts;
  if (consumeToken(Token::kw_if)) {
    stmts.push_back(parseConditionalStmt());
    while (consumeToken(Token::kw_else)) {
      if (consumeToken(Token::kw_if)) {
        stmts.push_back(parseConditionalStmt());
      } else {
        stmts.push_back(parseStmt());
      }
    }
  }
  return std::make_unique<ConditionalBlock>(std::move(stmts));
}

std::unique_ptr<DeclStmt> Parser::parseDeclStmt()  {
  auto decl = parseDecl();
  expectToken(Token::new_line, "new line");
  return std::make_unique<DeclStmt>(std::move(decl));
}

std::unique_ptr<ExprStmt> Parser::parseExprStmt() {
  auto expr = parseExpr();
  expectToken(Token::new_line, "new line");
  return std::make_unique<ExprStmt>(std::move(expr));
}

std::vector<std::unique_ptr<Stmt>> Parser::parseStmtList()  {
  std::vector<std::unique_ptr<Stmt>> elements;
  while(token_.is(Token::new_line)) consume();
  if (token_.isAny({Token::r_brace, Token::eof})) return elements;
  while(token_.is(Token::new_line)) consume();
  elements.push_back(parseStmt());
  while(token_.is(Token::new_line)) consume();
  while (token_.isNot(Token::r_brace) && token_.isNot(Token::eof)) {
    elements.push_back(parseStmt());
    while(token_.is(Token::new_line)) consume();
  }
  return elements;
}

std::unique_ptr<WhileLoop> Parser::parseWhileLoop()  {
  expectToken(Token::kw_while, "while");
  auto expr = parseExpr();
  auto stmt = parseCompoundStmt();
  return std::make_unique<WhileLoop>(std::move(expr), std::move(stmt));
}

std::unique_ptr<ReturnStmt> Parser::parseReturnStmt() {
  expectToken(Token::kw_return, "return");
  if (consumeToken(Token::new_line)) return std::make_unique<ReturnStmt>(nullptr);
  auto expr = parseExpr();
  expectToken(Token::new_line, "new line");
  return std::make_unique<ReturnStmt>(std::move(expr));
}

std::unique_ptr<CompilationUnit> Parser::parseCompilationUnit() {
  return std::make_unique<CompilationUnit>(parseStmtList());
}
