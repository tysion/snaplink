#include "base62.hpp"

#include <algorithm>

using namespace std::string_view_literals;

namespace snaplink::common {

std::string Base62Encode(int64_t num) {
    constexpr auto kChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"sv;
    constexpr auto kBase = 62;
    constexpr auto kSize = 7;

    if (num < 0) {
        throw std::invalid_argument("negative number");
    }

    std::string result;
    result.reserve(kSize);

    do {
        result.push_back(kChars[num % kBase]);
        num /= kBase;
    } while (num > 0);

    std::reverse(result.begin(), result.end());
    return result;
}

}  // namespace snaplink::common