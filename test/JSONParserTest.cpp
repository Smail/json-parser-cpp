#include <JSONParser.h>

#include <array>
#include <format>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
void print_result(std::string_view input, const std::string_view::iterator& begin,
                  const std::string_view::iterator& end, std::optional<T>&& result, auto expected) {
    constexpr auto kPassed = "\033[42mPASSED\033[0m";
    constexpr auto kFailed = "\033[41mFAILED\033[0m";

    if (result.has_value()) {
        std::cout << std::format("{} - Input: \"{}\" - Expected: \"{}\" - Result: \"{}\"\n",
                                 kPassed, input, expected, result.value());
    } else {
        std::cout << std::format("{} - Input: \"{}\" - Expected: \"{}\"\n", kFailed, input,
                                 expected);
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

    print_result(null, cursor, end, json::parse_null(cursor, end), null);
}

void test_parse_boolean() {
    {
        std::string_view true_ = "true";
        auto cursor = true_.begin();
        auto end = true_.end();
        print_result(true_, cursor, end, json::parse_boolean(cursor, end), true_);
        if (cursor != end) throw std::runtime_error("Not equal");
    }
    {
        std::string_view false_ = "false";
        auto cursor = false_.begin();
        auto end = false_.end();
        print_result(false_, cursor, end, json::parse_boolean(cursor, end), false_);
        if (cursor != end) throw std::runtime_error("Not equal");
    }
}

void test_parse_number() {
    using TestSet = std::pair<std::string_view, double>;

    constexpr std::array<TestSet, 7> test_correct{
        TestSet{"1", 1},    {"+1", 1},    {"-1", -1},
        {"938475", 938475}, {"00000", 0}, {"34589.23234", 34589.23234},
        {"13e+14", 13e+14},
    };

    for (const auto& [input, expected] : test_correct) {
        auto cursor = input.begin();
        auto end = input.end();

        print_result(input, cursor, end, json::parse_number(cursor, end), auto{expected});
    }
}

void test_parse_string() {
    constexpr std::array<std::string_view, 5> test_correct{
        "\"\"",
        "\"1\"",
        "\"this is a string\"",
        R"("escape \" me \" please")",
        R"("escape \" me \" please with some null and true and false 123 string")",
    };

    for (const auto& input : test_correct) {
        auto cursor = input.begin();
        auto end = input.end();

        print_result(input, cursor, end, json::parse_string(cursor, end), input);
    }
}

void test_parse_object() {
    std::vector<std::string> inputs{
        "{}",
        R"({"key": 1})",
        R"({"key": { "subobj" : 1 }})",
    };
    for (const auto& input : inputs) {
        std::cout << "INPUT: " << input << std::endl;
        auto result = json::parse_object(input);
        if (!result.has_value()) {
            std::cerr << "FAILED" << std::endl;
            return;
        }

        std::cout << "SUCCESS" << std::endl;
        std::cout << result.value() << std::endl;
    }
}

void test_parse_array() {
    std::vector<std::string> inputs{
        "[1, 2, 3, 4]",
        "[1, [2, 3], 4]",
        "[1, true, null, \"string\"]",
    };

    for (const auto& input : inputs) {
        std::cout << "INPUT: " << input << std::endl;
        auto result = json::parse_array(input);
        if (!result.has_value()) {
            std::cerr << "FAILED" << std::endl;
            return;
        }

        std::cout << "SUCCESS" << std::endl;
        std::cout << result.value() << std::endl;
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

    system("pause");
}
