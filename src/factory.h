#ifndef FACTORY_H
#define FACTORY_H

#include <memory>
#include <optional>

namespace Tui {
class Block;
class HLayout;
class VLayout;
class Divider;
class Text;

std::unique_ptr<Block> make_block(std::optional<uint32_t> width = std::nullopt);
std::unique_ptr<HLayout> make_horizontal_layout();
std::unique_ptr<VLayout> make_vertical_layout();
std::unique_ptr<Divider> make_divider(const Text& text);
} // namespace Tui

#endif // FACTORY_H