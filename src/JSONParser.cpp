#include "JSONParser.h"

#include <sstream>
#include <string>

namespace json {
auto parse_null(std::string_view::iterator& cursor, const std::string_view::iterator& end)
    -> std::optional<std::string_view> {
    if (*cursor != 'n') return std::nullopt;
    if ((cursor + 1) == end || *++cursor != 'u') return std::nullopt;
    if ((cursor + 1) == end || *++cursor != 'l') return std::nullopt;
    if ((cursor + 1) == end || *++cursor != 'l') return std::nullopt;

    cursor += 1;
    return "null";
}

auto parse_null(std::string_view json) -> std::optional<std::string_view> {
    auto cursor = json.begin();
    return parse_null(cursor, json.end());
}

auto parse_boolean(std::string_view::iterator& cursor, const std::string_view::iterator& end)
    -> std::optional<bool> {
    if (*cursor == 't') {
        if ((cursor + 1) == end || *++cursor != 'r') return std::nullopt;
        if ((cursor + 1) == end || *++cursor != 'u') return std::nullopt;
        if ((cursor + 1) == end || *++cursor != 'e') return std::nullopt;

        cursor += 1;
        return true;
    } else if (*cursor == 'f') {
        if ((cursor + 1) == end || *++cursor != 'a') return std::nullopt;
        if ((cursor + 1) == end || *++cursor != 'l') return std::nullopt;
        if ((cursor + 1) == end || *++cursor != 's') return std::nullopt;
        if ((cursor + 1) == end || *++cursor != 'e') return std::nullopt;

        cursor += 1;
        return false;
    }

    return std::nullopt;
}

auto parse_boolean(std::string_view json) -> std::optional<bool> {
    auto cursor = json.begin();
    return parse_boolean(cursor, json.end());
}

auto parse_number(std::string_view::iterator& cursor, const std::string_view::iterator& end)
    -> std::optional<double> {
    std::stringstream ss{};

    if (*cursor == '-' || *cursor == '+') ss << *cursor++;
    if (!std::isdigit(*cursor)) return std::nullopt;

    auto is_valid_number_char = [&cursor, &end]() {
        char c = *cursor;
        auto next = [&cursor](int offset) { return *(cursor + offset); };
        auto has_next = [&cursor, &end](int offset) { return (cursor + offset) != end; };

        if (std::isdigit(*cursor)) return true;
        if (c == '.' && has_next(1) && std::isdigit(next(1))) return true;
        // FIXME: Possible out of bounds if next(-1) is executed on first char, e.g. "+123"
        if ((c == '+' || c == '-') && (next(-1) == 'e' || next(-1) == 'E')) return true;
        if ((c == 'e' || c == 'E') && has_next(1)) {
            char next_c = next(1);

            if ((next_c == '+' || next_c == '-') && has_next(2)) {
                next_c = next(2);
            }

            if (std::isdigit(next_c)) {
                return true;
            }
        }

        return false;
    };

    for (; cursor != end && is_valid_number_char(); ++cursor) {
        ss << *cursor;
    }

    return std::stod(ss.str());
}

auto parse_number(std::string_view json) -> std::optional<double> {
    auto cursor = json.begin();
    return parse_number(cursor, json.end());
}
}  // namespace json
