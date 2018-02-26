#ifndef PARSE_PARSER_RESULT
#define PARSE_PARSER_RESULT

template <typename ASTNode> class ParserResult {
public:
  ASTNode* tree;
  bool error;
  ParserResult() : tree{nullptr}, error{true} {}
  ParserResult(ASTNode* node) : tree{node}, error{node == nullptr} {}
  ParserResult(ASTNode* node, bool error) : tree{node}, error{node == nullptr} {}

  bool isError() {
    return error;
  }
  ASTNode* getNode() { return tree; }
};
#endif
