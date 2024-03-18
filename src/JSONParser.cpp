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

auto parse_string(std::string_view::iterator& cursor, const std::string_view::iterator& end)
    -> std::optional<std::string_view> {
    if (*cursor != '"') return std::nullopt;

    std::stringstream ss{};
    cursor += 1;
    char last_char = '"';
    ss << '"';

    for (; cursor != end; ++cursor) {
        ss << *cursor;

        if (*cursor == '"' && last_char != '\\') {
            cursor += 1;
            break;
        }

        last_char = *cursor;
    }

    return ss.str();
}

auto parse_string(std::string_view json) -> std::optional<std::string_view> {
    auto cursor = json.begin();
    return parse_string(cursor, json.end());
}

std::ostream& operator<<(std::ostream& os, const Object& obj) {
    os << "{\n";
    int i = 0;
    for (const auto& [key, value] : obj.map) {
        os << '\t' << key << ": " << value << (i++ < obj.map.size() ? ",\n" : "\n");
    }
    return (os << '}');
}

std::ostream& operator<<(std::ostream& os, const Array& array) {
    os << '[';
    for (int i = 0; i < array.data.size(); ++i) {
        os << array.data[i] << ((i + 1 < array.data.size()) ? ", " : "");
    }
    return (os << ']');
}

std::ostream& operator<<(std::ostream& os, const Value& value) {
    std::visit(
        [&os](auto& x) {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, Object> || std::is_same_v<T, std::string_view> ||
                          std::is_same_v<T, double> || std::is_same_v<T, Array>) {
                os << x;
            } else if constexpr (std::is_same_v<T, bool>) {
                os << std::boolalpha << x;
            } else {
                static_assert(false, "Missing variant implementation");
            }
        },
        value);

    return os;
}
}  // namespace json
