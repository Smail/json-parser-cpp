#include "JSONParser.h"

#include <sstream>
#include <string>

namespace json {
Object::Object(std::initializer_list<std::unordered_map<std::string, Value>::value_type> xs)
    : map{xs} {}

Array::Array(std::initializer_list<Value> xs) : data{xs} {}

std::string_view::iterator& skip_whitespace(std::string_view::iterator& cursor,
                                            const std::string_view::iterator& end) {
    // Iterate over whitespace
    while (cursor != end && std::isspace(*cursor)) cursor += 1;
    return cursor;
}

auto parse_null(std::string_view::iterator& cursor, const std::string_view::iterator& end)
    -> std::optional<std::string> {
    if (*cursor != 'n') return std::nullopt;
    if ((cursor + 1) == end || *++cursor != 'u') return std::nullopt;
    if ((cursor + 1) == end || *++cursor != 'l') return std::nullopt;
    if ((cursor + 1) == end || *++cursor != 'l') return std::nullopt;

    cursor += 1;
    return "null";
}

auto parse_null(std::string_view json) -> std::optional<std::string> {
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
    -> std::optional<std::string> {
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

auto parse_string(std::string_view json) -> std::optional<std::string> {
    auto cursor = json.begin();
    return parse_string(cursor, json.end());
}

// NOLINTNEXTLINE(*-no-recursion)
[[nodiscard]] auto parse_key_value(std::string_view::iterator& cursor,
                                   const std::string_view::iterator& end)
    -> std::optional<std::pair<std::string, Value>> {
    // Iterate over whitespace
    if (skip_whitespace(cursor, end) == end) return std::nullopt;

    auto key = parse_string(cursor, end);
    if (!key.has_value()) return std::nullopt;

    // Iterate over whitespace
    if (skip_whitespace(cursor, end) == end) return std::nullopt;

    // Check for colon literal
    if (*cursor != ':' || ++cursor == end) return std::nullopt;

    // Iterate over whitespace
    if (skip_whitespace(cursor, end) == end) return std::nullopt;

    auto value = parse_value(cursor, end);
    if (!value.has_value()) return std::nullopt;

    return std::pair<std::string, Value>{key.value(), value.value()};
}

// NOLINTNEXTLINE(*-no-recursion)
auto parse_object(std::string_view::iterator& cursor, std::string_view::iterator const& end)
    -> std::optional<Object> {
    if (*cursor != '{') return std::nullopt;

    Object object{};
    for (size_t depth = 0; cursor != end; ++cursor) {
        if (*cursor == '{') {
            depth += 1;
            continue;
        }

        skip_whitespace(cursor, end);

        // Break loop if closing brace for scope was reached
        if (*cursor == '}' && --depth == 0) {
            cursor += 1;
            break;
        }

        auto entry = parse_key_value(cursor, end);
        if (!entry.has_value()) return std::nullopt;
        auto [key, value] = entry.value();
        object.map.insert({key, value});
    }

    return object;
}

// NOLINTNEXTLINE(*-no-recursion)
auto parse_object(std::string_view json) -> std::optional<Object> {
    auto cursor = json.begin();
    return parse_object(cursor, json.end());
}

// NOLINTNEXTLINE(*-no-recursion)
auto parse_array(std::string_view::iterator& cursor, const std::string_view::iterator& end)
    -> std::optional<Array> {
    if (*cursor != '[') return std::nullopt;
    if (++cursor == end) return std::nullopt;

    std::string input(cursor, end);

    Array array{};
    for (; cursor != end;) {
        // Iterate over whitespace
        if (skip_whitespace(cursor, end) == end) return std::nullopt;

        if (*cursor == ']') {
            cursor += 1;
            break;
        }

        if (*cursor == ',') {
            cursor += 1;
            continue;
        }

        auto value = parse_value(cursor, end);
        if (!value.has_value()) return std::nullopt;

        array.data.push_back(value.value());
    }

    return array;
}

auto parse_array(std::string_view json) -> std::optional<Array> {
    auto cursor = json.begin();
    return parse_array(cursor, json.end());
}

// NOLINTNEXTLINE(*-no-recursion)
[[nodiscard]] auto parse_value(std::string_view::iterator& cursor,
                               const std::string_view::iterator& end) -> std::optional<Value> {
    if (*cursor == '{') return parse_object(cursor, end);
    if (*cursor == '"') return parse_string(cursor, end);
    if (*cursor == '+' || *cursor == '-' || std::isdigit(*cursor)) return parse_number(cursor, end);
    if (*cursor == 'n') return parse_null(cursor, end);
    if (*cursor == 't' || *cursor == 'f') return parse_boolean(cursor, end);
    if (*cursor == '[') return parse_array(cursor, end);

    return std::nullopt;
}

auto parse_value(std::string_view json) -> std::optional<Value> {
    auto cursor = json.begin();
    return parse_value(cursor, json.end());
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
            if constexpr (std::is_same_v<T, Object> || std::is_same_v<T, std::string> ||
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

bool operator==(const Value& lhs, const Value& rhs) {
    if (lhs.index() != rhs.index()) return false;
    if (std::holds_alternative<bool>(lhs)) return std::get<bool>(lhs) == std::get<bool>(rhs);
    if (std::holds_alternative<std::string>(lhs))
        return std::get<std::string>(lhs) == std::get<std::string>(rhs);
    if (std::holds_alternative<Object>(lhs))
        return std::get<Object>(lhs).map == std::get<Object>(rhs).map;
    if (std::holds_alternative<Array>(lhs))
        return std::get<Array>(lhs).data == std::get<Array>(rhs).data;
    if (std::holds_alternative<double>(lhs)) return std::get<double>(lhs) == std::get<double>(rhs);

    return false;
}
}  // namespace json
