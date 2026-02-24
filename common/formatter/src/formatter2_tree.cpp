#include "../inc/formatter2_tree.h"

#include "common/log/log.h"

#include <algorithm>
#include <iostream>
#include <cassert>

Formatter2Tree::Formatter2Tree(const std::string& source, const TSNode& root) {
  // Two-step AST construction:
  // 1. Clean up the TSNode tree into the Formatter2CleanTree
  // 2. 

  int top_level_count = ts_node_child_count(root);
  this->top_level_clean_nodes = std::vector<Formatter2CleanNode>(top_level_count);

  for (int i = 0; i < top_level_count; i++) {
    this->top_level_clean_nodes[i] = Formatter2CleanNode();

    clean_ts_tree(source, ts_node_child(root, i), this->top_level_clean_nodes[i]);
  }
}

std::string ts_node_source(const std::string& source, const TSNode& tsnode) {
  uint32_t start = ts_node_start_byte(tsnode);
  uint32_t end = ts_node_end_byte(tsnode);
  return source.substr(start, end - start);
};

void Formatter2Tree::clean_ts_tree(
  const std::string& source,
  const TSNode& tsnode,
  Formatter2CleanNode& node,
  std::optional<std::string> node_prefix
) {
  uint32_t start = ts_node_start_byte(tsnode);
  uint32_t end = ts_node_end_byte(tsnode);
  int ts_child_count = ts_node_child_count(tsnode);
  const std::string ts_type = ts_node_type(tsnode);

  // Initialize a bunch of the node's fields
  node.metadata.source_start = start;
  node.metadata.source_end = end;

  if (node_prefix) {
    node.prefix = node_prefix;
  }

  if (ts_child_count > 0) {
    // We'll resize this later, so it's okay to overallocate here
    node.inner_nodes = std::make_optional(std::vector<Formatter2CleanNode>(ts_child_count));
  }

  node.debug_tstype = ts_type;

  TSPoint pstart = ts_node_start_point(tsnode);
  TSPoint pend = ts_node_end_point(tsnode);

  std::cout << "  Cleaning " << ts_type << "[(" << pstart.row << ", " << pstart.column << "):(" << pend.row << ", " << pend.column << ")] -> " << ts_child_count << " tschildren" << std::endl;

  // First, let's clean up the prefixes.
  // All prefixes have two or three children: the prefix, an optional gap, and the stuff it's prefixing.
  // Therefore, the only thing we want is the last child
  // There are four possible types:
  // - `quoting_lit`, like for 'symbols
  // - `quasi_quoting_lit`, like for `(macro stuff)
  // - `unquoting_lit`, like for macro ,args
  // - `unquote_splicing_lit`, like for macro ,@rest args
  // In these cases, we call this function with the *same* node but the appropriate *child* tsnode
  if (ts_type == "quoting_lit"
   || ts_type == "quasi_quoting_lit"
   || ts_type == "unquoting_lit"
   || ts_type == "unquote_splicing_lit") {
    const TSNode& prefix_node = ts_node_child(tsnode, 0);
    const TSNode& form_node = ts_node_child(tsnode, ts_child_count - 1);

    const std::string prefix = ts_node_source(source, prefix_node);
    clean_ts_tree(source, form_node, node, prefix);

    // Nothing else to do here!
    return;
  }

  // Node is either a genuine list or a defined node type.
  if (ts_type == "list_lit") {
    // First and lass children are '(' and ')' respectively,
    // let's remove those so we're left with only the actual content.
    node.inner_nodes.value().erase(node.inner_nodes.value().begin());
    node.inner_nodes.value().pop_back();

    bool used_head = false;

    if (ts_child_count > 2) {
      const std::string head = ts_node_source(source, ts_node_child(tsnode, 1));
  
      const std::vector<std::pair<std::vector<std::string>, Formatter2CleanNodeType>> node_type_heads = {
        { { "let", "let*", "mlet", "mlet*" }, Formatter2CleanNodeType::LET },
        { { "define", "defconstant" }, Formatter2CleanNodeType::DEFINE },
        { { "define-perm" }, Formatter2CleanNodeType::DEFINE_PERM },
        { { "define-extern", }, Formatter2CleanNodeType::DEFINE_EXTERN },
        { { "declare-type" }, Formatter2CleanNodeType::DECL_TYPE },
        { { "defun", "desfun", "defun-debug" }, Formatter2CleanNodeType::DEFUN },
        { { "defun-recursive", "defun-debug-recursive" }, Formatter2CleanNodeType::DEFUN_RECURSIVE },
        { { "defun-extern" }, Formatter2CleanNodeType::DEFUN_EXTERN },
        { { "def-mips2c" }, Formatter2CleanNodeType::DEF_MIPS2C },
        { { "defmethod-mips2c" }, Formatter2CleanNodeType::DEFMETHOD_MIPS2C },
        { { "defmethod" }, Formatter2CleanNodeType::DEFMETHOD },
        { { "defbehavior" }, Formatter2CleanNodeType::DEFBEHAVIOR },
        { { "defmacro", "defsmacro" }, Formatter2CleanNodeType::DEFMACRO },
        { { "deftype" }, Formatter2CleanNodeType::DEFTYPE },
        { { "defstate" }, Formatter2CleanNodeType::DEFSTATE },
        { { "defpart" }, Formatter2CleanNodeType::DEFPART },
        { { "defpartgroup" }, Formatter2CleanNodeType::DEFPARTGROUP },
        { { "def-art-elt" }, Formatter2CleanNodeType::DEF_ART_ELT },
        { { "def-joint-node" }, Formatter2CleanNodeType::DEF_JOINT_NODE },
        { { "def-tex" }, Formatter2CleanNodeType::DEF_TEX },
        { { "defskelgroup" }, Formatter2CleanNodeType::DEFSKELGROUP },
        { { "def-actor" }, Formatter2CleanNodeType::DEF_ACTOR },
        { { "in-package" }, Formatter2CleanNodeType::IN_PACKAGE },
        { { "bundles" }, Formatter2CleanNodeType::BUNDLE_LIST },
        { { "require" }, Formatter2CleanNodeType::REQUIRE }
      };
  
      for (uint32_t i = 0; i < node_type_heads.size(); i++) {
        auto heads_for_type = node_type_heads[i].first;
        if (std::find(heads_for_type.begin(), heads_for_type.end(), head) != heads_for_type.end()) {
          node.type = node_type_heads[i].second;
          used_head = true;
  
          break;
        }
      }
    }

    if (!used_head) {
      // We haven't found any known values,
      // so we must run a heuristic to choose between a simple list and a function call.
      // A list must:
      // - Have only one type of element
      // - Have at least two elements, or zero elements.

      if (ts_child_count - 2 < 1) {
        // List has no elements. It cannot be a function call.
        node.type = Formatter2CleanNodeType::LIST;
      } else if (ts_child_count == 3) {
        // List has exactly one element. It has to be a function call
        node.type = Formatter2CleanNodeType::FUNC_CALL;
      } else {
        const std::string list_type = ts_node_type(ts_node_child(tsnode, 1));
        bool same_type = true;

        for (int i = 2; i < ts_child_count - 2; i++) {
          if (ts_node_type(ts_node_child(tsnode, i)) != list_type) {
            same_type = false;
            break;
          }
        }

        if (same_type == true) {
          node.type = Formatter2CleanNodeType::LIST;
        }
      }

      if (node.type != Formatter2CleanNodeType::LIST) {
        node.type = Formatter2CleanNodeType::FUNC_CALL;
      }
    }

    // This node has children! Let's construct them too.
    // (Still avoiding the parentheses!)
    int base_idx = used_head ? 2 : 1;
    for (int i = base_idx; i < ts_child_count - 2; i++) {
      node.inner_nodes.value()[i - base_idx] = Formatter2CleanNode();
      clean_ts_tree(source, ts_node_child(tsnode, i), node.inner_nodes.value()[i - base_idx]);
    }

    // We're done here!
    return;
  }

  // Let's handle comments next.
  if (ts_type == "comment") {
    // There are two special comments:
    // ;;--FORMATTER OFF--
    // ;;--FORMATTER ON--
    // These disable and reenable the formatter respectively, for the section of code they are in.
    // For now though, we just have to assign the correct type to them.
    const std::string comment_source = ts_node_source(source, tsnode);

    if (comment_source == ";;--FORMATTER OFF--") {
      node.type = Formatter2CleanNodeType::FORMATTER_OFF;
    } else if (comment_source == ";;--FORMATTER ON--") {
      node.type = Formatter2CleanNodeType::FORMATTER_ON;
    } else {
      node.type = Formatter2CleanNodeType::COMMENT;
    }

    // Comments don't have anything else going on, carry on.
    return;
  }

  // Literals!
  if (ts_type == "bool_lit") {
    node.type = Formatter2CleanNodeType::LIT_BOOL;
  } else if (ts_type == "num_lit") {
    node.type = Formatter2CleanNodeType::LIT_NUMBER;
  } else if (ts_type == "str_lit") {
    node.type = Formatter2CleanNodeType::LIT_STRING;
  } else if (ts_type == "char_lit") {
    node.type = Formatter2CleanNodeType::LIT_CHAR;
  } else if (ts_type == "sym_lit") {
    node.type = Formatter2CleanNodeType::LIT_SYMBOL;
  } else if (ts_type == "kwd_lit") {
    node.type = Formatter2CleanNodeType::LIT_KEYWORD;
  }


  if (node.type == Formatter2CleanNodeType::UNKNOWN) {
    // Lastly, if all else fails, we leave the node as unknown, print an error, and hope it's fine.
    // Unknown nodes will not be formatted for fear of breaking something.
    std::cout << "Unknown node type: " << ts_type << std::endl;
  }

  return;
}

void Formatter2Tree::print_nodes() {
  for (uint32_t i = 0; i < this->top_level_clean_nodes.size(); i++) {
    print_node(0, this->top_level_clean_nodes[i]);
  }
}

void Formatter2Tree::print_node(int indent_level, const Formatter2CleanNode node) {
  const int INDENT_STEP = 2;

  std::string indent = std::string(indent_level * INDENT_STEP, ' ');

  std::string_view type_string = node_type_to_string(node.type);

  std::string prefix = node.prefix ? node.prefix.value() : "";

  uint32_t child_count = node.inner_nodes ? node.inner_nodes.value().size() : 0;
  std::cout << indent << "- " << prefix << type_string;
  
  if (node.debug_tstype) {
    std:: cout << " (" << node.debug_tstype.value() << ")";
  } 

  std::cout << " -> " << child_count << " children\n";

  if (node.inner_nodes) {
    for (uint32_t i = 0; i < node.inner_nodes.value().size(); i++) {
      if (node.debug_tstype) {
        print_node(indent_level + 1, node.inner_nodes.value()[i]);
      }
    }
  }
}
