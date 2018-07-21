#ifndef PARSER_H
#define PARSER_H

#include <deque>

#include "Basic/Token.h"
#include "Parse/Lexer.h"
#include "Parse/Operator.h"
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

  //shared_ptr<OperatorNode> parseOperator(int);
  bool parseTerminal(int type, std::string str, bool expect);
  bool consumeToken(int type);
  Token expectToken(int type, std::string);
  Token expectToken(std::vector<int> types, std::string);

  bool acceptToken(int type);
  bool consumeOperator(string);

  static shared_ptr<Type> makeType(std::string);
  shared_ptr<Type> parseType();
  shared_ptr<TypeIdentifier> parseTypeIdentifier();
  shared_ptr<TypeLabel> parseTypeLabel();
  shared_ptr<LabeledType> parseLabeledType();
  shared_ptr<TypeList> parseTupleTypeElementList();
  shared_ptr<TupleType> parseTupleType();
  shared_ptr<FunctionType> parseFunctionType();
  shared_ptr<Type> parseTupleOrFunctionType();
  shared_ptr<ListType> parseListType();
  shared_ptr<MapType> parseMapType();
  shared_ptr<Type> parseListOrMapType();

  static shared_ptr<Decl> makeDecl(std::string);
  static shared_ptr<Decl> makeTypeDecl(std::string);
  static shared_ptr<Decl> makeFuncDecl(std::string);

  shared_ptr<TypeDecl> parseTypeDecl();
  shared_ptr<FuncDecl> parseUndefFuncDecl();

  shared_ptr<Decl> parseDecl();
  shared_ptr<TypeAlias> parseTypeAlias();
  shared_ptr<VarDecl> parseVarDecl();
  shared_ptr<LetDecl> parseLetDecl();
  shared_ptr<FuncDecl> parseFuncDecl();
  shared_ptr<ParamDecl> parseParamDecl();
  shared_ptr<ParamDeclList> parseParamDeclList();

  shared_ptr<Expr> parseExpr(int precedence = OperatorTable::size());
  shared_ptr<Expr> parseParenthesizedExpr();
  shared_ptr<OperatorExpr> parseOperatorExpr(int precedence);
  shared_ptr<Expr> parseBinaryExpr(int precedence);
  shared_ptr<Expr> parseInfixNone(int p);
  shared_ptr<Expr> parseInfixLeft(int p);
  shared_ptr<Expr> parseInfixRight(int p);
  shared_ptr<Expr> parseUnaryExpr();
  shared_ptr<Expr> parseValueExpr();
  shared_ptr<IntegerExpr> parseIntegerExpr();
  shared_ptr<DoubleExpr> parseDoubleExpr();
  shared_ptr<StringExpr> parseStringExpr();
  shared_ptr<ExprLabel> parseExprLabel();
  shared_ptr<ListExpr> parseListExpr();
  shared_ptr<LabeledExpr> parseLabeledExpr();
  shared_ptr<Expr> parseLabeledExprOrExpr();
  shared_ptr<Expr> parseIdentifierOrFunctionCallOrAccessor();
  shared_ptr<ExprList> parseExprList();
  shared_ptr<IdentifierExpr> parseIdentifier();
  shared_ptr<Expr> parseTupleExpr();
  shared_ptr<FunctionCall> parseFunctionCall();
  shared_ptr<ExprList> parseFunctionParameters();

  shared_ptr<Stmt> makeStmt(std::string text);
  shared_ptr<Stmt> parseStmt();
  shared_ptr<ReturnStmt> parseReturnStmt();
  shared_ptr<CompoundStmt> parseCompoundStmt();
  shared_ptr<WhileLoop> parseWhileLoop();
  shared_ptr<ConditionalStmt> parseConditionalStmt();
  shared_ptr<ConditionalStmtList> parseConditionalStmtList();
  shared_ptr<StmtList> parseStmtList();
  shared_ptr<DeclStmt> parseDeclStmt();
  shared_ptr<ExprStmt> parseExprStmt();


  /*
  shared_ptr<StmtList> parseParamList();
  shared_ptr<IfStmt> parseIfStmt();
  shared_ptr<WhileStmt> parseWhileStmt();
  shared_ptr<ReturnStmt> parseReturnStmt();
  shared_ptr<ExprStmt> parseExprStmt();
  shared_ptr<StmtList> parseStmtList();
  shared_ptr<BlockStmt> parseBlockStmt();
  shared_ptr<Program> parseProgram();
  */
};

#endif
