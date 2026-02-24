#pragma once

#include <string>

#include "tree_sitter/api.h"

enum Formatter2Style {
  Flow,
  Hang,
  Bracey
};

struct Formatter2Config {
  Formatter2Style style;
  int indent_step;
};

struct Formatter2Data {
  Formatter2Config config;
  int indent_level;
  int alignment;
};

class Formatter2 {
  public:
    static void create_visualize_ts_tree(const std::string& source);
    static void visualize_ts_tree(const std::string& source, int level, const TSNode& node);
    static void create_visualize_fmt_tree(const std::string& source);
};


struct TreeSitterParserDeleter2 {
  void operator()(TSParser* ptr) const { ts_parser_delete(ptr); }
};

struct TreeSitterTreeDeleter2 {
  void operator()(TSTree* ptr) const { ts_tree_delete(ptr); }
};
