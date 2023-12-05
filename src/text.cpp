#include "text.h"

namespace Tui {
Text::Text() = default;

std::string Text::str() const {
    std::string s;
    for (const auto& t : tokens) {
        s += t.string;
    }
    return s;
}

Text::Length Text::size() const {
    return length;
}

Text::RawLength Text::size_r() const {
    return RawLength(tokens.size());
}

Text Text::substr(RawIndex start_r) const {
    return substr(start_r, RawLength {UINT32_MAX});
}

Text Text::substr(RawIndex start_r, RawLength len_r) const {
    Text text;
    uint32_t n = 0;
    for (uint32_t i = start_r; i < tokens.size() && n < len_r; i++) {
        text += tokens[i];
        n++;
    }

    return text;
}

Text Text::substr(RawIndex start_r, Length len) const {
    Text text;
    uint32_t n = 0;
    for (uint32_t i = start_r; i < tokens.size() && n < len; i++) {
        text += tokens[i];
        n += tokens[i].size;
    }
    return text;
}

std::optional<Text::RawIndex> Text::find(char ch, RawIndex pos_r, RawLength len_r) const {
    uint32_t n = 0;
    for (uint32_t i = pos_r; i < tokens.size() && n < len_r; i++, n++) {
        const auto& token = tokens[i];
        if (!token.string.empty() && token.string[0] == ch)
            return RawIndex {i};
    }
    return std::nullopt;
}

Text Text::rpad(Length len, char ch) const {
    if (length >= len)
        return *this;
    return *this + Text {std::string(len - length, ch)};
}

Text Text::lpad(Length len, char ch) const {
    if (length >= len)
        return *this;
    return Text {std::string(len - length, ch)} + *this;
}

Text& Text::operator+=(const Text& s) {
    tokens.insert(tokens.end(), s.tokens.begin(), s.tokens.end());
    length = length + s.length;
    return *this;
}

Text operator+(const Text& t1, const Text& t2) {
    Text text;
    text += t1;
    text += t2;
    return text;
}
} // namespace Tui