#pragma once

#include <optional>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace json {
class Object;
class Array;

using Value = std::variant<std::string_view, Object, double, bool, Array>;

struct Object {
    friend std::ostream& operator<<(std::ostream& os, const Object& obj);
    std::unordered_map<std::string_view, Value> map{};
};

struct Array {
    friend std::ostream& operator<<(std::ostream& os, const Array& obj);
    std::vector<Value> data{};
};

[[nodiscard]] auto parse_null(std::string_view::iterator& cursor,
                              const std::string_view::iterator& end)
    -> std::optional<std::string_view>;

[[nodiscard]] auto parse_null(std::string_view json) -> std::optional<std::string_view>;

[[nodiscard]] auto parse_boolean(std::string_view::iterator& cursor,
                                 const std::string_view::iterator& end) -> std::optional<bool>;

[[nodiscard]] auto parse_boolean(std::string_view json) -> std::optional<bool>;

[[nodiscard]] auto parse_number(std::string_view::iterator& cursor,
                                const std::string_view::iterator& end) -> std::optional<double>;

[[nodiscard]] auto parse_number(std::string_view json) -> std::optional<double>;

[[nodiscard]] auto parse_string(std::string_view::iterator& cursor,
                                const std::string_view::iterator& end)
    -> std::optional<std::string_view>;

[[nodiscard]] auto parse_string(std::string_view json) -> std::optional<std::string_view>;

std::ostream& operator<<(std::ostream& os, const Value& value);
}  // namespace json
