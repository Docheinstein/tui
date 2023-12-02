#ifndef TEXT_H
#define TEXT_H

#include "token.h"
#include <string>
#include <vector>

namespace Tui {
class Text {
public:
    Text();

    template <typename T>
    Text(T&& value) {
        std::string s;

        if constexpr (std::is_integral_v<T>) {
            s = std::to_string(value);
        } else {
            s = value;
        }

        tokens.reserve(s.size());
        for (const auto& c : s) {
            tokens.emplace_back(c);
        }
        size_ = s.size();
    }

    [[nodiscard]] std::string str() const;
    [[nodiscard]] uint32_t size() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] Text substr(uint32_t start, uint32_t len) const;
    [[nodiscard]] Text rpad(uint32_t len, char ch = ' ') const;

    Text& operator+=(const Token& t);
    Text& operator+=(const Text& s);
    friend Text operator+(const Text& s1, const Text& s2);

protected:
    std::vector<Token> tokens;
    uint32_t size_ {};
};
} // namespace Tui
#endif // TEXT_H