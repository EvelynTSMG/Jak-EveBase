#pragma once

#include <optional>

#include "formatter2_tree.h"

// Unlike when I first made an AST in Rust, C++ does not have algebraic data types
// So this file defines types for all the different node types that need to store their data nicely

struct Formatter2NodeMetadata {
  bool force_chop = false;
  int complexity = 0;
  uint32_t source_start = 0;
  uint32_t source_end = 0;
};

// Marker type
class Formatter2Node {
  Formatter2NodeMetadata metadata;
};

// The only node that doesn't have a 
class Formatter2NodeComment : Formatter2Node {
  std::string comment;
};

class Formatter2BaseNode {
  // Each node can have one comment on its line
  std::optional<Formatter2NodeComment> comment;
};
