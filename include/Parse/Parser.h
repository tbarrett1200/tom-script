#ifndef PARSER_H
#define PARSER_H

#include <deque>

#include "Parse/Token.h"
#include "Parse/Lexer.h"
#include "Parse/Operator.h"
#include "AST/ASTNode.h"
#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/Stmt.h"

class Parser {
  SourceCode* source;
  Lexer lexer;
  std::deque<Token> tokens;
  static std::vector<int> exprStartTokens;

public:
  Parser(SourceCode* source);

  Token token(int index=0);
  void consume();
  std::string report(Token, std::string);

  void consumeUntil(std::vector<int> types);

  //unique_ptr<OperatorNode> parseOperator(int);
  bool parseTerminal(int type, std::string str, bool expect);
  bool consumeToken(int type);
  Token expectToken(int typem, std::string);

  bool acceptToken(int type);
  bool consumeOperator(string);

  static unique_ptr<Type> makeType(std::string);
  unique_ptr<Type> parseType();
  unique_ptr<TypeIdentifier> parseTypeIdentifier();
  unique_ptr<TypeLabel> parseTypeLabel();
  unique_ptr<LabeledType> parseLabeledType();
  unique_ptr<TypeList> parseTupleTypeElementList();
  unique_ptr<TupleType> parseTupleType();
  unique_ptr<FunctionType> parseFunctionType(bool decl);
  unique_ptr<Type> parseTupleOrFunctionType();
  unique_ptr<ListType> parseListType();
  unique_ptr<MapType> parseMapType();
  unique_ptr<Type> parseListOrMapType();
  unique_ptr<Decl> parseDecl();
  unique_ptr<TypeAlias> parseTypeAlias();
  unique_ptr<VarDecl> parseVarDecl();
  unique_ptr<LetDecl> parseLetDecl();
  unique_ptr<FuncDecl> parseFuncDecl();

  unique_ptr<Expr> parseExpr(int precedence = OperatorTable::size());
  unique_ptr<Expr> parseParenthesizedExpr();
  unique_ptr<OperatorExpr> parseOperatorExpr(int precedence);
  unique_ptr<Expr> parseBinaryExpr(int precedence);
  unique_ptr<Expr> parseInfixNone(int p);
  unique_ptr<Expr> parseInfixLeft(int p);
  unique_ptr<Expr> parseInfixRight(int p);
  unique_ptr<Expr> parseUnaryExpr();
  unique_ptr<Expr> parseValueExpr();
  unique_ptr<IntegerExpr> parseIntegerExpr();
  unique_ptr<DoubleExpr> parseDoubleExpr();
  unique_ptr<StringExpr> parseStringExpr();
  unique_ptr<ExprLabel> parseExprLabel();
  unique_ptr<LabeledExpr> parseLabeledExpr();
  unique_ptr<Expr> parseLabeledExprOrExpr();
  unique_ptr<Expr> parseIdentifierOrFunctionCall();
  unique_ptr<ExprList> parseExprList();
  unique_ptr<IdentifierExpr> parseIdentifier();
  unique_ptr<Expr> parseTupleExpr();
  unique_ptr<FunctionCall> parseFunctionCall();
  unique_ptr<TupleExpr> parseFunctionParameters();


  /*
  unique_ptr<StmtList> parseParamList();
  unique_ptr<Stmt> parseStmt();
  unique_ptr<IfStmt> parseIfStmt();
  unique_ptr<WhileStmt> parseWhileStmt();
  unique_ptr<ReturnStmt> parseReturnStmt();
  unique_ptr<ExprStmt> parseExprStmt();
  unique_ptr<StmtList> parseStmtList();
  unique_ptr<BlockStmt> parseBlockStmt();
  unique_ptr<Program> parseProgram();
  */
};

#endif
