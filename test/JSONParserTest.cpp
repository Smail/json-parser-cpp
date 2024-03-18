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
        std::cout << std::format("{} - Start iterator is not at the end\n", kFailed);
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

int main(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);

    test_parse_null();
    test_parse_boolean();
}
