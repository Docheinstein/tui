#ifndef TOKEN_H
#define TOKEN_H

#include <cstdint>
#include <string>
#include <utility>

namespace Tui {
struct Token {
    Token(const std::string& string, bool size);
    Token(std::string&& string, bool size);
    explicit Token(char ch);

    std::string string;
    bool size {};
};
} // namespace Tui
#endif // TOKEN_H