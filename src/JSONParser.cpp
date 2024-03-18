#include "JSONParser.h"

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
}  // namespace json
