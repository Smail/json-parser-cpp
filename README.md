# JSON Parser

A JSON parser for modern C++.

## Usage

### C++

```cpp
#include <JSONParser.h>

int main() {
    std::string_view json = "[1, 2, true, null, { \"key\": \"value\" }]";
    if (std::optional<json::Value> json_value = json::parse_value(json); json_value.has_value()) {
        // ...
    }
}
```

### CMake

```cmake
add_executable(example example.cpp)

add_subdirectory(json-parser-cpp)
target_link_libraries(example PUBLIC JSONParser::JSONParser)
```

## Backlog

- [x] Parse JSON.
- [ ] Improve error handling.
- [ ] Add support for indexing a JSON object.
