#if !defined(UTFUTILS_H)
#   define UTFUTILS_H

// Standard library
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

#if __cplusplus < 202002L // < c++20
    using char8_t = unsigned char;
#endif

//----------------------------------------------------INTERFACE----------------------------------------------------//

namespace utf::converters {
    std::basic_string<char8_t> utf8_from_utf16(const std::basic_string_view<char16_t>& utf16_str);
    std::basic_string<char8_t> utf8_from_utf32(const std::basic_string_view<char32_t>& utf32_str);

    std::basic_string<char16_t> utf16_from_utf8(const std::basic_string_view<char8_t>& utf8_str);
    std::basic_string<char16_t> utf16_from_utf32(const std::basic_string_view<char32_t>& utf32_str);

    std::basic_string<char32_t> utf32_from_utf8(const std::basic_string_view<char8_t>& utf8_str);
    std::basic_string<char32_t> utf32_from_utf16(const std::basic_string_view<char16_t>& utf16_str);

} // namespace utf

//--------------------------------------------------IMPLEMENTATION--------------------------------------------------//
#if defined IMPLEMENT_UTFUTILS

namespace utf {
    namespace constants {
        // About these magical numbers:
        // https://en.wikipedia.org/wiki/UTF-16#Code_points_from_U+010000_to_U+10FFFF
        constexpr uint16_t high_surrogate_start = 0xD800;
        constexpr uint8_t high_surrogate_marker = 0xD800 >> 10;
        constexpr uint16_t low_surrogate_start = 0xDC00;
        constexpr uint8_t low_surrogate_marker = 0xDC00 >> 10;
        constexpr uint32_t supplementary_plane_offset = 0x10000;
    }
    
    // About "surrogates":
    // https://en.wikipedia.org/wiki/UTF-16#Code_points_from_U+010000_to_U+10FFFF
    constexpr bool is_high_surrogate(const char16_t ch) {
        return constants::high_surrogate_marker == ch >> 10;
    }
    constexpr bool is_low_surrogate(const char16_t ch) {
        return constants::low_surrogate_marker == ch >> 10;
    }
}

std::basic_string<char8_t> utf::converters::utf8_from_utf16(const std::basic_string_view<char16_t>& utf16_str) {
    std::basic_string<char32_t> code_points = utf32_from_utf16(utf16_str);

    return {};
}

std::basic_string<char16_t> utf::converters::utf16_from_utf8(const std::basic_string_view<char8_t>& utf8_str) {
    return {};
}

std::basic_string<char32_t> utf::converters::utf32_from_utf16(const std::basic_string_view<char16_t>& utf16_str) {
    // vector of UTF-32 characters (code points)
    std::basic_string<char32_t> code_points;

    bool was_double_character = false;
    for (auto character_it = utf16_str.begin(); character_it < utf16_str.end(); character_it++) {
        // skip this character, because we handled it last iteration
        if (was_double_character) {
            was_double_character = false;
            continue;
        }
        // get this character
        char16_t this_character = *character_it;

        // check if this character is the last one
        const bool exists_next_character = (character_it + 1) != utf16_str.end();
        char16_t next_character;

        // get next character if there is one
        if (exists_next_character) {
            next_character = *(character_it + 1);
        }

        // if can be part of double character
        if (is_high_surrogate(this_character)) {
            // if there is no next character we add this as code point
            if (!exists_next_character) {
                code_points.push_back(this_character);
                continue;
            }

            // if next character is not part of the double character we add this as code point
            if (!is_low_surrogate(next_character)) {
                code_points.push_back(this_character);
                continue;
            }

            // it turned out we have a double character
            was_double_character = true;

            // do decoding "double UTF-16" -> UTF-32:
            // https://en.wikipedia.org/wiki/UTF-16#Code_points_from_U+010000_to_U+10FFFF
            char32_t high_code_point = (this_character - constants::high_surrogate_start) << 10;
            char16_t low_code_point = next_character - constants::low_surrogate_start;
            char32_t code_point = high_code_point + low_code_point + constants::supplementary_plane_offset;
            code_points.push_back(code_point);

            continue;
        }

        // if not a double character add this as next code point
        code_points.push_back(this_character);
    }
    // return resulting string
    return code_points;
}

#endif // defined IMPLEMENT_UTFUTILS
#endif // !defined(UTFUTILS_H)