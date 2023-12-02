#include "block.h"

namespace Tui {
Block::Block(std::optional<uint32_t> width) :
    Node(Node::Type::Block),
    width(width) {
}

Block& Tui::Block::operator<<(const Text& text) {
    if (lines.empty())
        lines.emplace_back();
    lines.back() += text;
    return *this;
}

Block& Tui::Block::operator<<(Block& (*manip)(Block&)) {
    return manip(*this);
}

Block& endl(Block& b) {
    b.lines.emplace_back();
    return b;
}

Block& operator<<(const std::unique_ptr<Block>& block, const Text& text) {
    return *block << text;
}
} // namespace Tui