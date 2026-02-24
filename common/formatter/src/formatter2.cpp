#include "../inc/formatter2.h"
#include "../inc/formatter2_tree.h"
#include "../inc/formatter2_nodes.h"
#include "formatter2_tree.cpp"

#include <iostream>
#include <memory>

// Declare the `tree_sitter_opengoal` function, which is
// implemented by the `tree-sitter-opengoal` library.
extern "C" {
  extern const TSLanguage* tree_sitter_opengoal();
}

void Formatter2::create_visualize_ts_tree(const std::string& source) {
  // Create a parser.
  std::shared_ptr<TSParser> parser(
    ts_parser_new(),
    TreeSitterParserDeleter2()
  );

  ts_parser_set_language(parser.get(), tree_sitter_opengoal());

  // Build a syntax tree based on source code stored in a string.
  std::shared_ptr<TSTree> tree(
    ts_parser_parse_string(parser.get(), NULL, source.c_str(), source.length()),
    TreeSitterTreeDeleter2()
  );

  visualize_ts_tree(source, 0, ts_tree_root_node(tree.get()));
}

void Formatter2::visualize_ts_tree(const std::string& source, int level, const TSNode& node) {
  uint32_t start = ts_node_start_byte(node);
  uint32_t end = ts_node_end_byte(node);
  std::string node_text = source.substr(start, end - start);

  uint32_t first_nl = node_text.find('\n');
  node_text = node_text.substr(0, first_nl);

  if (node_text.ends_with('\n')) {
    node_text.pop_back();
  }

  std::string indent = std::string(level * 2, ' ');
  std::cout << indent << "- " << ts_node_type(node) << ": '" << node_text << "'\n";

  int len = ts_node_child_count(node);
  for (int i = 0; i < len; i++) {
    const TSNode& child = ts_node_child(node, i);
    visualize_ts_tree(source, level + 1, child);
  }
}

void Formatter2::create_visualize_fmt_tree(const std::string& source) {
  // Create a parser.
  std::shared_ptr<TSParser> parser(
    ts_parser_new(),
    TreeSitterParserDeleter2()
  );

  ts_parser_set_language(parser.get(), tree_sitter_opengoal());

  // Build a syntax tree based on source code stored in a string.
  std::shared_ptr<TSTree> tree(
    ts_parser_parse_string(parser.get(), NULL, source.c_str(), source.length()),
    TreeSitterTreeDeleter2()
  );

  Formatter2Tree* fmt_tree = new Formatter2Tree(source, ts_tree_root_node(tree.get()));

  std::cout << std::endl;

  fmt_tree->print_nodes();
}
