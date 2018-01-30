# tom-script
```Swift
var alpha: Int;
var beta: Int;

func add(a: Int, b: Int) -> void {
  return a+b
}
```

# Diagnostic Error Reporting
```
test:5:0: error: expected ;
func (a: Int, b: Int) -> void {
^
test:8:15: error: expected ;
    return a+c b;
               ^
```

# Unit Test Coverage
## Completed
* SourceCode
* Token
## In Progress
* Lexer
## Future
* ErrorReporter
* Parser
* SymbolAnnotator
* SymbolChecker
* Symbols
* Tree
* Visitor
```
make: `test' is up to date.
Running main() from gtest_main.cc
[==========] Running 18 tests from 3 test cases.
[----------] Global test environment set-up.
[----------] 7 tests from Lexer
[ RUN      ] Lexer.Constructor
[       OK ] Lexer.Constructor (0 ms)
[ RUN      ] Lexer.lexKeywords
[       OK ] Lexer.lexKeywords (0 ms)
[ RUN      ] Lexer.lexStringLiteral
[       OK ] Lexer.lexStringLiteral (0 ms)
[ RUN      ] Lexer.lexNumber
[       OK ] Lexer.lexNumber (0 ms)
[ RUN      ] Lexer.lexIdentifier
[       OK ] Lexer.lexIdentifier (0 ms)
[ RUN      ] Lexer.lexPunctuation
[       OK ] Lexer.lexPunctuation (0 ms)
[ RUN      ] Lexer.lexEOF
[       OK ] Lexer.lexEOF (0 ms)
[----------] 7 tests from Lexer (0 ms total)

[----------] 4 tests from SourceCode
[ RUN      ] SourceCode.Constructor
[       OK ] SourceCode.Constructor (0 ms)
[ RUN      ] SourceCode.getLineCount
[       OK ] SourceCode.getLineCount (0 ms)
[ RUN      ] SourceCode.getLine
[       OK ] SourceCode.getLine (0 ms)
[ RUN      ] SourceCode.getChar
[       OK ] SourceCode.getChar (0 ms)
[----------] 4 tests from SourceCode (0 ms total)

[----------] 7 tests from Token
[ RUN      ] Token.DefaultConstructor
[       OK ] Token.DefaultConstructor (0 ms)
[ RUN      ] Token.ParameterConstructor
[       OK ] Token.ParameterConstructor (0 ms)
[ RUN      ] Token.is
[       OK ] Token.is (0 ms)
[ RUN      ] Token.isNot
[       OK ] Token.isNot (0 ms)
[ RUN      ] Token.isAny
[       OK ] Token.isAny (0 ms)
[ RUN      ] Token.isIntLiteral
[       OK ] Token.isIntLiteral (0 ms)
[ RUN      ] Token.isDoubleLiteral
[       OK ] Token.isDoubleLiteral (0 ms)
[----------] 7 tests from Token (0 ms total)

[----------] Global test environment tear-down
[==========] 18 tests from 3 test cases ran. (0 ms total)
[  PASSED  ] 18 tests.
[Finished in 0.063s]
```

# Formal Language Specification (BNF)
This language is LL(1)... however, the current language specification is not
properly LL(1), but can be rewritten as such. The handwritten recursive descent parser
uses some procedural logic to simplify the parsing process. This can be noticed
in the param-list definition.

Note: Make sure to update this specification as the Parser is modified.
```
<program>     ::= <stmt-list> <eof>
<stmt-list>   ::= <stmt> <stmt-list>
<stmt>        ::= <expr-stmt> | <block-stmt> | <if-stmt> | <while-stmt> |
                  <return-stmt> | <var-decl> | <func-decl>
<expr-stmt>   ::= <expr> ;
<expr>        ::= <binary-expr>
<binary-expr> ::= <value-expr> <operator> <binary-expr>
<value-expr>  ::= <identifier> | INTEGER | DOUBLE | STRING | ( <expr> )
<identifier>
<operator>    ::= OPERATOR
<block-stmt>  ::= { <stmt-list> }
<if-stmt>     ::= if ( <expr> ) <block-stmt>
<while-stmt>  ::= while ( <expr> ) <block-stmt>
<return-stmt> ::= return <expr> ;
<var-decl>    ::= var <identifier> : <type> ;
<type>        ::= <identifier>
<func-decl>   ::= func <identifier> ( <param-list> ) -> <type> <block-stmt>
<param-list>  ::= <param-decl> | <param-decl> , <param-list>
<param-decl>  ::= <identifier> : <type>
```
