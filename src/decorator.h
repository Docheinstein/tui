#ifndef DECORATOR_H
#define DECORATOR_H

#include "text.h"

namespace Tui {
struct Decorator : Text {
    explicit Decorator(std::string&& s);
};
} // namespace Tui
#endif // DECORATOR_H