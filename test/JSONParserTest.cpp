#include <JSONParser.h>

#include <array>
#include <format>
#include <iostream>
#include <string>
#include <vector>

using TestValue = std::pair<std::string_view, json::Value>;

template <typename T>
void print_result(const std::string_view::iterator& begin, const std::string_view::iterator& end,
                  std::optional<T>&& result, TestValue&& test_value) {
    constexpr auto kPassed = "\033[42mPASSED\033[0m";
    constexpr auto kFailed = "\033[41mFAILED\033[0m";

    const auto& [input, expected] = test_value;

    if (result.has_value()) {
        if (result.value() == expected) {
            std::cout << kPassed << " - Input: \"" << input << "\" - Expected: \"" << expected
                      << "\" - Result: \"" << result.value() << "\"\n";
        } else {
            std::cout << kFailed << " - Input: \"" << input << "\" - Expected: \"" << expected
                      << "\" - Result: \"" << result.value() << "\"\n";
        }
    } else {
        std::cout << kFailed << " - PARSER ERROR - Input: \"" << input << "\" - Expected: \""
                  << expected << "\n";
    }

    if (begin == end) {
        std::cout << std::format("{} - Start iterator matches end\n", kPassed);
    } else {
        std::cout << std::format("{} - Start iterator is not at the end! Distance: {}\n", kFailed,
                                 std::distance(begin, end));
    }
}

void test_parse_null() {
    std::string_view null = "null";
    auto cursor = null.begin();
    auto end = null.end();

    print_result(cursor, end, json::parse_null(cursor, end), {null, "null"});
}

void test_parse_boolean() {
    {
        std::string_view true_ = "true";
        auto cursor = true_.begin();
        auto end = true_.end();
        print_result(cursor, end, json::parse_boolean(cursor, end), {true_, true});
    }
    {
        std::string_view false_ = "false";
        auto cursor = false_.begin();
        auto end = false_.end();
        print_result(cursor, end, json::parse_boolean(cursor, end), {false_, false});
    }
}

void test_parse_number() {
    std::vector<TestValue> tests{
        {"1", 1.},           {"+1", 1.},    {"-1", -1.},
        {"938475", 938475.}, {"00000", 0.}, {"34589.23234", 34589.23234},
        {"13e+14", 13e+14},
    };

    for (const auto& [input, expected] : tests) {
        auto cursor = input.begin();
        auto end = input.end();

        print_result(cursor, end, json::parse_number(cursor, end), TestValue{input, expected});
    }
}

void test_parse_string() {
    std::vector<std::string_view> tests{
        "\"\"",
        "\"1\"",
        "\"this is a string\"",
        R"("escape \" me \" please")",
        R"("escape \" me \" please with some null and true and false 123 string")",
    };

    for (const auto& input : tests) {
        auto cursor = input.begin();
        auto end = input.end();
        print_result(cursor, end, json::parse_string(cursor, end),
                     TestValue{std::string{input}, std::string{input}});
    }
}

void test_parse_object() {
    std::vector<TestValue> tests{
        {"{}", json::Object{}},
        {R"({"key": 1})", json::Object{{"\"key\"", 1.}}},
        {R"({"key": { "hello" : "world" }})",
         json::Object{{"\"key\"", json::Object{{"\"hello\"", "\"world\""}}}}},
    };

    for (const auto& [input, expected] : tests) {
        auto cursor = input.begin();
        auto end = input.end();

        print_result(cursor, end, json::parse_object(cursor, end), TestValue{input, expected});
    }
}

void test_parse_array() {
    std::vector<TestValue> inputs{
        {"[1, 2, 3, 4]", json::Array{1., 2., 3., 4.}},
        {"[1, [2, 3], 4]", json::Array{1., json::Array{2., 3.}, 4.}},
        {"[1, true, null, \"string\"]", json::Array{1., true, "null", "\"string\""}},
    };

    for (const auto& [input, expected] : inputs) {
        auto cursor = input.begin();
        auto end = input.end();

        print_result(cursor, end, json::parse_array(cursor, end), TestValue{input, expected});
    }
}

int main(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);

    test_parse_null();
    test_parse_boolean();
    test_parse_number();
    test_parse_string();
    test_parse_object();
    test_parse_array();
}
