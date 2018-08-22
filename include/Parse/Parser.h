#ifndef PARSER_H
#define PARSER_H

#include <deque>

#include "Basic/Token.h"
#include "Basic/CompilerException.h"

#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Type.h"
#include "AST/Stmt.h"

#include "Parse/Lexer.h"
#include "Parse/Operator.h"


/**
 * A class which parses a syntax tree from a stream of tokens. It throws
 * a CompilerException upon error. The compiler exception can caught and
 * printed cleanly to the command line using an ErrorReporter.
 *
 * TODO: This class currently returns std::shared_ptr instaces of the nodes.
 *       seeing as ownership of the nodes are clear - they have one and only one
 *       parent - it may be wise to convert to unique_ptr, which are more
 *       efficient and reduce likely-hood of leaks by eliminating change of
 *       cyclical dependency.
 *
 * TODO: This class currently has no root node - a program node.
 *
 * TODO: Should this class own the Lexer, SourceCode, and DeQue? Would
 *       a single parameter - tokenStream - be better?
 *
 * TODO: Should AST nodes maintain a list of possible start tokens for more
 *       easily modifiable parsing. Rather than maintaining its own list of
 *       possible start nodes, each method could just reference the AST nodes
 *       start token types.
 *
 * TODO: Take expression type initialization out of parser! it should not go
 *       here. ScopeManager should be its own stage?
 */
class Parser {
private:

  //===-----------------------------  Fields ------------------------------===//

  // The source code to parse
  std::shared_ptr<SourceFile> source;

  // Turns source code into tokens
  std::unique_ptr<Lexer> lexer;

  // Outut of lexer
  std::deque<Token> tokens;

  Token token_;

  //===-------------------------- Internal Use  ---------------------------===//

  // The possible start tokens of expressions
  static std::vector<int> exprStartTokens;

public:
  Parser(std::shared_ptr<SourceFile> source);


  //===-------------------------  Helper Methods --------------------------===//

  void consume() {
    token_ = lexer->next();
  }

  void consumeUntil(std::vector<int> types);

  /**
   * Consumes a token of the given type and the given lexeme if it exists. If
   * the third parameter 'expect' is true, this method will throw an exception
   * otherwise, it will just return false upon error. This method does not
   * return the token, so it can only be used if the resulting lexeme is not
   * needed. However, because the expected lexeme must be given as a parameter,
   * this information is not needed anyway.
   */
  bool parseTerminal(int type, const char* str, bool expect);

  /**
   * Consumes a token of the given type if it exists in the token stream.
   * Returns true if found and false otherwise. This is a good method to use
   * in a control flow statement when the actual lexeme is not relevant. e.g.
   * punctutation.
   *
   *  while (consumeToken(Token::comma)) {
   *    ...
   *  }
   *
   */
  bool consumeToken(int type);

  /**
   * Consumes and returns a token of the given type if it exists, otherwise
   * throws a CompilerExpection of the form "expected foo but found bar".
   * This is a good method to use when a token is absolutely required for the
   * parse to succeed, and the actual lexeme is relevant. The second parameter
   * is the name that the error message should use when referring the the
   * token. It has no impact on the actual passing or failing of the parse.
   */
  Token expectToken(int type, const char *lexeme);

  /**
   * Consumes and returns a token of any of given TypeAlias if it exists, otherwise
   * throws a CompilerExpection of the form "expected foo but found bar".
   * This is a good method to use when a token is absolutely required for the
   * parse to succeed, and the actual lexeme is relevant. The second parameter
   * is the name that the error message should use when referring the the
   * token. It has no impact on the actual passing or failing of the parse.
   */
  Token expectToken(std::vector<int> types, const char *lexeme);

  /**
   * Checks for a token of the given type. If it exists, consumes it, and
   * return true. Otherwise, it returns false and does not raise an error
   */
  bool acceptToken(int type);

  /**
   * Checks for a operator_id with the given lexeme. If it exists, consumes it, and
   * return true. Otherwise, it returns false. This method is equivelant to
   * parseTerminal(Token::Kind::operator_id, str, false), but uses a shorter,
   * more readable syntax that is used for convenience.
   */
  bool consumeOperator(const char *lexeme);


  //===--------------------------  Type Parsers ---------------------------===//


  /**
   *
   */
  const Type* parseType();
  /**
   *
   */
  const Type* parseTypeIdentifier();
  /**
   *
   */
  std::vector<const Type*> parseTupleTypeElementList();
  /**
   *
   */
  const Type* parseReferenceOrSliceType();

  const TupleType* parseTupleType();
  /**
   *
   */
  const FunctionType* parseFunctionType();
  /**
   *
   */
  const Type* parseTupleOrFunctionType();
  /**
   *
   */
  const ListType* parseListType();
  /**
   *
   */
  const MapType* parseMapType();


  const PointerType* parsePointerType();

  /**
   *
   */
  const Type* parseListOrMapType();

  //===----------------------  Declaration Parsers ------------------------===//


  /**
   *
   */
  std::unique_ptr<FuncDecl> parseUndefFuncDecl();

  /**
   *
   */
  std::unique_ptr<Decl> parseDecl();

  /**
   *
   */
  std::unique_ptr<TypeAlias> parseTypeAlias();

  /**
   *
   */
  std::unique_ptr<Decl> parseVarDecl();

  /**
   *
   */
  std::unique_ptr<LetDecl> parseLetDecl();

  /**
   *
   */
  std::unique_ptr<FuncDecl> parseFuncDecl();

  /**
   *
   */
  std::unique_ptr<ParamDecl> parseParamDecl();

  std::unique_ptr<ExternFuncDecl> parseExternFuncDecl();

  /**
   *
   */
  std::vector<std::unique_ptr<ParamDecl>> parseParamDeclList();

  //===-----------------------  Expression Parsers ------------------------===//

  /**
   *
   */
  std::unique_ptr<Expr> parseExpr(int precedence = OperatorTable::size());

  /**
   * Parses an expression nested in parenthesis. This method simply wraprs the
   * parseExpr method with a check for parenthesis. This is not its own
   * discrete node type, but rather a flavour of expressions.
   *
   * <expr> := '(' <expr> ')'
   */
  std::unique_ptr<Expr> parseParenthesizedExpr();

  /**
   * Because of the shortcomings of LL parsing, operator precedence parsing is
   * a different beast. It features left recursion in its grammer, which can't
   * be easily handled with a purely recursive approach. The additition of
   * operator precedence associativity and precedence adds another layer
   * of complexity.
   *
   * The grammer is given an arbitrary nubmer of precedence levels, which are
   * declared in a static OperatorTable singleton. Each precedence level has
   * one and only one arity (unary, binary etc), fixity (infix, prefix postfix),
   * and associativity (left, right, none). At this time, not all permutations
   * are supported because not all permutations are logical or commonly used.
   *
   */
  Token parseOperator(int precedence);
  /**
   *
   */
  std::unique_ptr<Expr> parseBinaryExpr(int precedence);

  /**
   *
   */
  std::unique_ptr<Expr> parseInfixNone(int precedence);

  /**
   *
   */
  std::unique_ptr<Expr> parseInfixLeft(int precedence);

  /**
   *
   */
  std::unique_ptr<Expr> parseInfixRight(int precedence);

  std::unique_ptr<Expr> parseAccessorExpr();
  /**
   *
   */
  std::unique_ptr<Expr> parseUnaryExpr();

  /**
   *
   */
  std::unique_ptr<Expr> parseValueExpr();

  /**
   *
   */
  std::unique_ptr<IntegerExpr> parseIntegerExpr();

  std::unique_ptr<BoolExpr> parseBoolExpr();

  unique_ptr<CharacterExpr> parseCharacterExpr();

  /**
   *
   */
  std::unique_ptr<DoubleExpr> parseDoubleExpr();

  /**
   * Parses a string literal
   */
  std::unique_ptr<StringExpr> parseStringExpr();

  /**
   * Parses a list expression, which is an expression list wrapped in square
   * brackets.
   */
  std::unique_ptr<ListExpr> parseListExpr();

  /**
   * Looks ahead in the token stream and returns either a function call,
   * identifier, or accessor expression depending on the context.
   */
  std::unique_ptr<Expr> parseIdentifierOrFunctionCall();

  /**
   * Parses a comma seperated list of expression. This is similar to argument
   * list, so is there really a need for both? Food for thought.
   *
   * <expr-list> := (<expr> ( ',' <expr> )* )?
   */
  std::vector<std::unique_ptr<Expr>> parseExprList();

  /**
   * Parses an identifier, which is just a thin wrapper arround a identifier
   * token.
   *
   * <identifier> := Token::identifier
   */
  std::unique_ptr<IdentifierExpr> parseIdentifier();

  /**
   * Parses a tuple expression, which is a comma seperated list of expressions
   * nested in parenthesis.
   *
   * <tuple-expr> := '(' <argument-list> ')'
   */
  std::unique_ptr<Expr> parseTupleExpr();

  /**
   * Parses a function call, which is an identifier token followed by a
   * parenthsis nested argument list. Note that differentiating between a
   * functional call is only possible with lookahead, so in a context where
   * function calls and identifiers are both possible, the method
   * 'parseFunctionCallOrAccessor' should be used. This method will lookahead
   * and parse the proper expressions.
   *
   * <function-call> := <identifier> '(' <argument-list> ')'
   */
  std::unique_ptr<FunctionCall> parseFunctionCall();

  /**
   * Parses a list of comma seperated expressions for use as arguments to a
   * function. There may not be a trailing comma. All commas must be followed
   * by an expression. The expression list may be empty, but only if the
   * next token in the stream is a valid argument list terminator. Currently,
   * the only possible terminator is a right parenthesis. If the right
   * paranthesis are found, the method will return an array with no elements.
   *
   * <function-arguments> := (<expr> ( ',' <expr> )* )?
   */
  std::vector<std::unique_ptr<Expr>> parseFunctionArguments();


  //===------------------------  Statement Parsers ------------------------===//


  /**
   * Parses a newline terminated statement from the token stream. The newline
   * character acts as the statement terminator... This means that there can
   * only be one statement per line. This method does not deal with newlines
   * directly, but instead defers all handling of newlines to the parsing of
   * specific statement types.
   *
   * The statement may be a ReturnStmt, a CompoundStmt, WhileLoop,
   * ConditionalStmtList, a DeclStmt, or an ExprStmt.
   *
   * <stmt> := <return-stmt> | <while-loop> | <conditional-stmt-list> |
               <decl-stmt> | <expr-stmt>
   *
   * Note: in parsing a ConditionalStmtList, one or more ConditionalStmts may
   *       be parsed. The must be parsed as a single block to ensure logical
   *       grouping.
   *
   * This method checks for Stmt derived classes by switching against their
   * possible start tokens. This method should be updated every time a derived
   * type is added or changed its grammer. This switch stmt must have every
   * possible proper start token in order to succeed.
   *
   * if the next token in the stream is not a statement start token, then
   * a compiler exception is thrown.
   *
   *
   */
  std::unique_ptr<Stmt> parseStmt();

  /**
   * Parses a return statment from the token stream. There may not
   * be any newlines before the statement, but there must be one after. This
   * method will consume at most one newline after the statement.
   *
   * <return-stmt> := 'return' <expr> <new-line>
   */
  std::unique_ptr<ReturnStmt> parseReturnStmt();

  /**
   * Parses a list of statements wrapped in braces from the token stream.
   * The compound statement may contain any of the statements parsed by the
   * parseStmt method. The compound statement may not have a newline preceding
   * it in the token stream, and it does not need one after. No newline
   * characters will be consumed after the compound statement.
   *
   * <compound-stmt> := '{' <stmt-list> '}'
   *
   * Note: logically, the return statement must be the last statement in a
   * block of statements. All statements after it are dead code and should
   * be eliminated. However, the parser does not handle this type of
   * semantic check. It is grammatically valid for a return statement to be in
   * the middle of a block, however it is not semantically correct. A semantic
   * checking stage should raise a warning if a return statement occurs in the
   * middle of a block.
   */
  std::unique_ptr<CompoundStmt> parseCompoundStmt();

  /**
   * Parses a while loop from the token stream. The while statement consists
   * of a keyword, 'while', a conditional expression, and a block statment.
   * The while loop may not be preceded by a newline in the input stream,
   * but must be terminated by a newline, which will be consumed.
   *
   * <while-loop> := 'while' <expr> <compound-stmt> <new-line>
   */
  std::unique_ptr<WhileLoop> parseWhileLoop();

  /**
   * Parses a conditional statment from the input stream. The conditional
   * statement may not be preceded by a newline character, and will not consume
   * and newline characters that follow it.
   *
   * <conditional-stmt> := 'if' <expr> <compound-stmt> |
   *                       'else' 'if' <expr> <compound-stmt> |
   *                       'else' <compound-stmt>
   *
   * TODO: It may be a good idea to split the logic for conditional statements
   *       into three seperate methods.
   *
   * Note: the ConditionalStmt returned by this method may not have all of its
   *       fields intialized. All valid parses will contain a statment, but an
   *       else stmt will not have a condition.
   *
   * Note: the expression must have a boolean type, but type-checking is not the
   *       job of the parser. A later semantic checking stage should make sure
   *       that the conditional statement has a boolean type.
   */
  std::unique_ptr<ConditionalStmt> parseConditionalStmt();

  /**
   * Parses a group of conditional statments. The first conditional statment
   * must be an if statement. The if statement can be optionally followed by
   * any number of else-if statements. An else statement is also optional, but
   * must come last in the list. The conditional statement block may not be
   * preceded by a newline character, but must be folled by one. The newline
   * at the end will be consumed.
   *
   * <conditional-stmt-list> := <contional-stmt>+
   */
   std::unique_ptr<ConditionalBlock> parseConditionalBlock();

  /**
   * Parses as many stmts as possible from the input stream. It will stop
   * parsing stmts when it reaches a stmt-list terminator. The two possible
   * terminators are a right brace - if the stmt-list appears in a compound
   * stmt, or an eof token - if the stmt-list is the program wide list. This
   * method defers to each derived implementation to strip newlines following
   * each statement, but must remove as many newlines as it finds before the
   * next statement or the stmt-list terminator token.
   *
   * <stmt-list> := (<new-line>* <stmt>)*
   *
   * Note: a statement list is not a tree node, but rather a stdlib container
   *       Standard containers contain a wide variety of efficient iterators
   *       that would not be practical to recreate at this time.
   */
  std::vector<std::unique_ptr<Stmt>> parseStmtList();

  /**
   * Parses a newline-terminated declaration from the token stream. This method
   * is a light wrapper around the parseDecl method. A declaration is allowed
   * to appear on its own to allow for lexical scoping - a declaration may only
   * be used by statements that follow it.
   * may no be precededed by a new-line.
   *
   * <decl-stmt> := <decl> <new-line>
   */
  std::unique_ptr<DeclStmt> parseDeclStmt();

  /**
   * Parses a newline-terminated expression from the token stream. This method
   * is a light wrapper around the parseExpr method. An expression is allowed
   * to appear on its own because of its possible side effects. The expression
   * may no be precededed by a new-line.
   *
   * <expr-stmt> := <expr> <new-line>
   */
  std::unique_ptr<ExprStmt> parseExprStmt();

  std::unique_ptr<CompilationUnit> parseCompilationUnit();

};

#endif
