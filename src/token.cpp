#include "token.h"

namespace Tui {
Token::Token(const std::string& string, bool size) :
    string(string),
    size(size) {
}

Token::Token(std::string&& string, bool size) :
    string(std::move(string)),
    size(size) {
}

Token::Token(char ch) :
    Token({ch}, true) {
}
} // namespace Tui