#include "decorator.h"

namespace Tui {
Decorator::Decorator(std::string&& s) :
    Text() {
    tokens.emplace_back(std::move(s), 0);
    size_ = 0;
}
} // namespace Tui
