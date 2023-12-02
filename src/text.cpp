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

uint32_t Text::size() const {
    return size_;
}

bool Text::empty() const {
    return size_ == 0;
}

Text Text::substr(uint32_t start, uint32_t len) const {
    if (start == 0 && len >= size_)
        return *this;

    Text text;
    for (uint32_t i = start; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        if (text.size() + token.size <= len) {
            text += token;
        }
    }
    return text;
}

Text Text::rpad(uint32_t len, char ch) const {
    if (size_ >= len)
        return *this;
    return *this + Text {std::string(len - size_, ch)};
}

Text& Text::operator+=(const Token& t) {
    tokens.push_back(t);
    size_ += t.size;
    return *this;
}

Text& Text::operator+=(const Text& s) {
    tokens.insert(tokens.end(), s.tokens.begin(), s.tokens.end());
    size_ += s.size_;
    return *this;
}

Text operator+(const Text& t1, const Text& t2) {
    Text text;
    text += t1;
    text += t2;
    return text;
}

} // namespace Tui