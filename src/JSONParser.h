#pragma once

#include <optional>
#include <string_view>

namespace json {
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
}  // namespace json
