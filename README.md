# JSON Parser

A JSON parser for modern C++.

## Usage

```cpp
#include <JSONParser.h>

int main() {
    std::string_view json = "[1, 2, true, null, { \"key\": \"value\" }]";
    if (std::optional<json::Value> json_value = json::parse_value(json); json_value.has_value()) {
        // ...
    }
}
```

## Backlog

- [x] Parse JSON.
- [ ] Improve error handling.
- [ ] Add support for indexing a JSON object.
