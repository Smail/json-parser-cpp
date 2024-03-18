#include <JSONParser.h>

#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

using TestValue = std::pair<std::string_view, json::Value>;

constexpr auto kPassed = "\033[42mPASSED\033[0m";
constexpr auto kFailed = "\033[41mFAILED\033[0m";

template <typename T>
[[nodiscard]] bool print_result(const std::string_view::iterator& begin,
                                const std::string_view::iterator& end, std::optional<T>&& result,
                                TestValue&& test_value) {
    const auto& [input, expected] = test_value;

    bool success = false;
    if (result.has_value()) {
        if (result.value() == expected) {
            std::cout << kPassed << " - Input: \"" << input << "\" - Expected: \"" << expected
                      << "\" - Result: \"" << result.value() << "\"\n";
            success = true;
        } else {
            std::cerr << kFailed << " - Input: \"" << input << "\" - Expected: \"" << expected
                      << "\" - Result: \"" << result.value() << "\"\n";
        }
    } else {
        std::cerr << kFailed << " - PARSER ERROR - Input: \"" << input << "\" - Expected: \""
                  << expected << "\n";
    }

    if (begin == end) {
        std::cout << std::format("{} - Start iterator matches end\n", kPassed);
    } else {
        std::cerr << std::format("{} - Start iterator is not at the end! Distance: {}\n", kFailed,
                                 std::distance(begin, end));
        success = false;
    }

    return success;
}

[[nodiscard]] bool test_parse_null() {
    std::string_view null = "null";
    auto cursor = null.begin();
    auto end = null.end();

    return print_result(cursor, end, json::parse_null(cursor, end), {null, "null"});
}

[[nodiscard]] bool test_parse_boolean_true() {
    std::string_view true_ = "true";
    auto cursor = true_.begin();
    auto end = true_.end();

    return print_result(cursor, end, json::parse_boolean(cursor, end), {true_, true});
}

[[nodiscard]] bool test_parse_boolean_false() {
    std::string_view false_ = "false";
    auto cursor = false_.begin();
    auto end = false_.end();

    return print_result(cursor, end, json::parse_boolean(cursor, end), {false_, false});
}

[[nodiscard]] bool test_parse_number() {
    std::vector<TestValue> tests{
        {"1", 1.},           {"+1", 1.},    {"-1", -1.},
        {"938475", 938475.}, {"00000", 0.}, {"34589.23234", 34589.23234},
        {"13e+14", 13e+14},
    };

    std::vector<bool> return_values{};
    for (const auto& [input, expected] : tests) {
        auto cursor = input.begin();
        auto end = input.end();

        return_values.push_back(
            print_result(cursor, end, json::parse_number(cursor, end), TestValue{input, expected}));
    }

    return std::ranges::all_of(return_values, [](bool b) { return b; });
}

[[nodiscard]] bool test_parse_string() {
    std::vector<std::string_view> tests{
        "\"\"",
        "\"1\"",
        "\"this is a string\"",
        R"("escape \" me \" please")",
        R"("escape \" me \" please with some null and true and false 123 string")",
    };

    std::vector<bool> return_values{};
    for (const auto& input : tests) {
        auto cursor = input.begin();
        auto end = input.end();
        return_values.push_back(print_result(cursor, end, json::parse_string(cursor, end),
                                             TestValue{std::string{input}, std::string{input}}));
    }

    return std::ranges::all_of(return_values, [](bool b) { return b; });
}

[[nodiscard]] bool test_parse_object() {
    std::vector<TestValue> tests{
        {"{}", json::Object{}},
        {R"({"key": 1})", json::Object{{"\"key\"", 1.}}},
        {R"({"key": { "hello" : "world" }})",
         json::Object{{"\"key\"", json::Object{{"\"hello\"", "\"world\""}}}}},
    };

    std::vector<bool> return_values{};
    for (const auto& [input, expected] : tests) {
        auto cursor = input.begin();
        auto end = input.end();

        return_values.push_back(
            print_result(cursor, end, json::parse_object(cursor, end), TestValue{input, expected}));
    }

    return std::ranges::all_of(return_values, [](bool b) { return b; });
}

[[nodiscard]] bool test_parse_array() {
    std::vector<TestValue> inputs{
        {"[1, 2, 3, 4]", json::Array{1., 2., 3., 4.}},
        {"[1, [2, 3], 4]", json::Array{1., json::Array{2., 3.}, 4.}},
        {"[1, true, null, \"string\"]", json::Array{1., true, "null", "\"string\""}},
    };

    std::vector<bool> return_values{};
    for (const auto& [input, expected] : inputs) {
        auto cursor = input.begin();
        auto end = input.end();

        return_values.push_back(
            print_result(cursor, end, json::parse_array(cursor, end), TestValue{input, expected}));
    }

    return std::ranges::all_of(return_values, [](bool b) { return b; });
}

[[nodiscard]] bool test_parse_json_files() {
    std::vector<std::string> file_names{"test.json", "test2.json"};

    std::vector<bool> return_values{};
    for (const auto& file_name : file_names) {
        std::ifstream file{file_name};
        if (!file.good()) {
            std::cerr << "File Error: " << file_name << std::endl;
            return_values.push_back(false);
            continue;
        }

        std::string json = (std::stringstream{} << file.rdbuf()).str();
        std::string_view json_view = json;
        auto cursor = json_view.begin();
        auto result = json::parse_value(cursor, json_view.end());

        if (result.has_value()) {
            std::cout << kPassed << " - File: " << file_name << std::endl;
            return_values.push_back(true);
        } else {
            std::cerr << kFailed << " - File: " << file_name << " - Unexpected symbol at index: "
                      << std::distance(json_view.begin(), cursor)
                      << " - Context: " << std::string_view(cursor, json_view.end()).substr(0, 100)
                      << std::endl;
            return_values.push_back(false);
        }
    }

    return true;
}

int main(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);

    std::vector<bool> return_values{};
    return_values.push_back(test_parse_null());
    return_values.push_back(test_parse_boolean_true());
    return_values.push_back(test_parse_boolean_false());
    return_values.push_back(test_parse_number());
    return_values.push_back(test_parse_string());
    return_values.push_back(test_parse_object());
    return_values.push_back(test_parse_array());
    return_values.push_back(test_parse_json_files());

    system("pause");

    return std::ranges::all_of(return_values, [](bool b) { return b; }) ? EXIT_SUCCESS
                                                                        : EXIT_FAILURE;
}
