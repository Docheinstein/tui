#ifndef TEXT_H
#define TEXT_H

#include "token.h"
#include "traits.h"
#include <optional>
#include <string>
#include <vector>

namespace Tui {
class Text {
public:
    using Length = Explicit<uint32_t, struct LengthTag>;
    using RawLength = Explicit<uint32_t, struct RawLengthTag>;
    using Index = Explicit<uint32_t, struct IndexTag>;
    using RawIndex = Explicit<uint32_t, struct RawIndexTag>;

    Text();

    template <typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, Token>, bool> = true>
    Text(T&& value) {
        tokens.emplace_back(std::forward<T>(value));
        length = value.size;
    }

    template <typename T, std::enable_if_t<std::negation_v<std::is_same<std::decay_t<T>, Token>>, bool> = true>
    Text(T&& value) {
        std::string s;

        if constexpr (std::is_integral_v<std::decay_t<T>>) {
            s = std::to_string(value);
        } else {
            s = std::forward<T>(value);
        }

        tokens.reserve(s.size());
        for (const auto& c : s) {
            tokens.emplace_back(c);
        }
        length = s.size();
    }

    template <typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, Token>>>
    Text& operator=(const Token& t) {
        tokens.push_back(t);
        length = t.size;
        return *this;
    }

    template <typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, Token>>>
    Text& operator+=(const Token& t) {
        tokens.push_back(t);
        length = length + t.size;
        return *this;
    }

    Text& operator+=(const Text& s);
    friend Text operator+(const Text& s1, const Text& s2);

    [[nodiscard]] std::string str() const;
    [[nodiscard]] RawLength size_r() const;
    [[nodiscard]] Length size() const;
    [[nodiscard]] Text substr(RawIndex start) const;
    [[nodiscard]] Text substr(RawIndex start, RawLength len) const;
    [[nodiscard]] Text substr(RawIndex start, Length len) const;
    [[nodiscard]] Text rpad(Length len, char ch = ' ') const;
    [[nodiscard]] Text lpad(Length len, char ch = ' ') const;
    [[nodiscard]] std::optional<RawIndex> find(char ch, RawIndex pos = RawIndex {0},
                                               RawLength len = RawLength {UINT32_MAX}) const;

protected:
    std::vector<Token> tokens;
    Length length {};
};
} // namespace Tui
#endif // TEXT_H