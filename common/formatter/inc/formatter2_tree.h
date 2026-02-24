#pragma once

#include <array>
#include <string_view>
#include <optional>
#include <vector>

#include "tree_sitter/api.h"

#include "formatter2_nodes.h"

enum Formatter2CleanNodeType {
  UNKNOWN,

  COMMENT,

  FORMATTER_OFF, // Special comment to disable the formatter
  FORMATTER_ON,  // Special comment to reenable the formatter

  // Literals
  LIT_BOOL,
  LIT_NUMBER, // We don't really care what type of number at this point
  LIT_STRING,
  LIT_CHAR,
  LIT_SYMBOL,
  LIT_KEYWORD, // has its paired value as a child for easier formatting

  LET, // also mlet and let*
  
  LIST, // Actual list like ('low 'medium 'high)
  
  FUNC_CALL, // Requires lookahead to choose between func call and symbol list
  
  // Soooo many defines
  DEFINE, // same as defconstant
  DEFINE_PERM, // define + type
  DEFINE_EXTERN,
  DECL_TYPE,
  DEFUN, // All of these have subtly different formatting, but they're largely the same
  DEFUN_RECURSIVE, // defun + return type
  DEFUN_EXTERN,
  DEF_MIPS2C,
  DEFMETHOD_MIPS2C,
  DEFMETHOD, // defun + type
  DEFBEHAVIOR, // defmethod + can take bindings
  DEFMACRO, // same as defsmacro and defgmacro
  DEFTYPE,
  DEFSTATE,
  DEFPART,
  DEFPARTGROUP,
  DEF_ART_ELT,
  DEF_JOINT_NODE,
  DEF_TEX,
  DEFSKELGROUP,
  DEF_ACTOR,

  // Preamble stuff
  IN_PACKAGE,
  BUNDLE_LIST,
  REQUIRE,

  COUNT
};

// The fact that this is the best way to convert enum variants to a string pains me
constexpr std::array<
  std::pair<Formatter2CleanNodeType, std::string_view>,
  static_cast<size_t>(Formatter2CleanNodeType::COUNT)
> node_type_strings = {{
  { Formatter2CleanNodeType::UNKNOWN, "unknown"},
  { Formatter2CleanNodeType::COMMENT, "comment"},
  { Formatter2CleanNodeType::FORMATTER_OFF, "Disable Formatter"},
  { Formatter2CleanNodeType::FORMATTER_ON, "Enable Formatter"},
  { Formatter2CleanNodeType::LIT_BOOL, "boolean"},
  { Formatter2CleanNodeType::LIT_NUMBER, "number"},
  { Formatter2CleanNodeType::LIT_STRING, "string"},
  { Formatter2CleanNodeType::LIT_CHAR, "char" },
  { Formatter2CleanNodeType::LIT_SYMBOL, "symbol"},
  { Formatter2CleanNodeType::LIT_KEYWORD, "keyword"},
  { Formatter2CleanNodeType::LET, "let"},
  { Formatter2CleanNodeType::LIST, "list"},
  { Formatter2CleanNodeType::FUNC_CALL, "call"},
  { Formatter2CleanNodeType::DEFINE, "define"},
  { Formatter2CleanNodeType::DEFINE_PERM, "define-perm"},
  { Formatter2CleanNodeType::DEFINE_EXTERN, "define-extern"},
  { Formatter2CleanNodeType::DECL_TYPE, "declare-type"},
  { Formatter2CleanNodeType::DEFUN, "defun"},
  { Formatter2CleanNodeType::DEFUN_RECURSIVE, "defun-recursive"},
  { Formatter2CleanNodeType::DEFUN_EXTERN, "defun-extern"},
  { Formatter2CleanNodeType::DEF_MIPS2C, "def-mips2c" },
  { Formatter2CleanNodeType::DEFMETHOD_MIPS2C, "defmethod-mips2c" },
  { Formatter2CleanNodeType::DEFMETHOD, "defmethod"},
  { Formatter2CleanNodeType::DEFBEHAVIOR, "defbehavior"},
  { Formatter2CleanNodeType::DEFMACRO, "defmacro"},
  { Formatter2CleanNodeType::DEFTYPE, "deftype"},
  { Formatter2CleanNodeType::DEFSTATE, "defstate"},
  { Formatter2CleanNodeType::DEFPART, "defpart"},
  { Formatter2CleanNodeType::DEFPARTGROUP, "defpartgroup"},
  { Formatter2CleanNodeType::DEF_ART_ELT, "def-art-elt"},
  { Formatter2CleanNodeType::DEF_JOINT_NODE, "def-joint-node"},
  { Formatter2CleanNodeType::DEF_TEX, "def-tex"},
  { Formatter2CleanNodeType::DEFSKELGROUP, "defskelgroup"},
  { Formatter2CleanNodeType::DEF_ACTOR, "def-actor"},
  { Formatter2CleanNodeType::IN_PACKAGE, "in-package"},
  { Formatter2CleanNodeType::BUNDLE_LIST, "bundle-list"},
  { Formatter2CleanNodeType::REQUIRE, "require"}
}};

constexpr std::string_view node_type_to_string(Formatter2CleanNodeType type) {  
  for (const auto& pair : node_type_strings) {  
    if (pair.first == type) {  
      return pair.second;  
    }  
  }

  return "???";
}

class Formatter2CleanNode {
  public:
    Formatter2NodeMetadata metadata;
    
    Formatter2CleanNodeType type = Formatter2CleanNodeType::UNKNOWN;
    std::optional<std::string> debug_tstype = {};
    std::optional<std::string> prefix = {};
    std::optional<std::string> head = {};

    std::optional<std::vector<Formatter2CleanNode>> inner_nodes = {};

    Formatter2CleanNode() = default;
    Formatter2CleanNode(const Formatter2NodeMetadata& _metadata) : metadata(_metadata) {};
};

class Formatter2Tree {
  public:
    Formatter2Tree(const std::string& source, const TSNode& root);
    std::vector<Formatter2Node> top_level_nodes;
    
    void print_nodes();
    
  private:
    std::vector<Formatter2CleanNode> top_level_clean_nodes;

    void clean_ts_tree(
      const std::string& source,
      const TSNode& tsnode,
      Formatter2CleanNode& node,
      std::optional<std::string> node_prefix = {}
    );

    void analyze_complexity();

    void print_node(int indent_level, const Formatter2CleanNode node);
};
