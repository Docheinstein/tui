#ifndef DECORATOR_H
#define DECORATOR_H

#include "text.h"

namespace Tui {
struct Decorator : Text {
    template <typename T>
    Decorator(T&& str) :
        Text() {
        tokens.emplace_back(std::forward<T>(str), 0);
        length = 0;
    }
};
} // namespace Tui
#endif // DECORATOR_H