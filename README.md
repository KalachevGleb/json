# JSON C++ Library

This repository contains a compact library for handling JSON data written in C++20.
It provides a comprehensive and intuitive API for constructing, parsing,
and modifying JSON objects.

## Features

This JSON library provides a straightforward and minimalistic approach for handling JSON data in C++. 
It focuses on small and medium-sized JSON data, providing a simple and intuitive API for common operations.

- **JSON Parsing and Serialization**: Convert JSON text to a JSON object and vice versa.
- **Data Access**: Directly access or modify JSON objects and arrays using standard C++ syntax, 
  such as `obj["key"]` for objects or `arr[index]` for arrays.
- **Type-Safe Operations**: Retrieve or set values in a type-safe manner, 
  with clear error handling for type mismatches.
- **Conversion Utilities**: Easily convert JSON objects to standard C++ types and containers,
  such as `std::map`, `std::vector`, `std::string`, etc.
- **Error Handling**: Provides clear and informative exceptions for common errors
  like type mismatches or out-of-range access.

These features cover the basic operations required to use JSON data effectively in 
a C++ environment, supporting typical use cases such as configuration files,
data interchange, and more.

## Directory Structure

```
json/
├── CMakeLists.txt  // CMake configuration for building the library
├── json.cpp        // Implementation file
└── json.h          // Header file with the JSON class and declarations
LICENSE             // MIT License
README.md           // This file
```

## Requirements

- C++20 compliant compiler (GCC, Clang, MSVC)
- CMake 3.12 or higher

## Usage

### Building Separate Static Library

To build the library, you can use CMake. Here is a quick set of commands to compile the library:

```bash
mkdir build
cd build
cmake ..
make
```

This will generate a static library that can be linked against your projects.

### Integrating into CMake Project

To integrate the JSON C++ Library into your CMake-based project, follow these steps:

1. **Copy the `json` directory into your project directory.**

2. **Include the library in your CMake configuration:**
   Add the following lines to your top-level `CMakeLists.txt`:

   ```cmake
   add_subdirectory(json)
   ```

3. **Link the library to your target:**
   Assuming the target is called `my_app`, modify your `CMakeLists.txt` to link against the JSON library:

   ```cmake
   target_link_libraries(my_app PRIVATE json_lib)
   ```
   This also adds the necessary include directory to the target.

4. **Include the header in your project files:**
   ```cpp
   #include "json.h"
   ```

### Example

Here's a creative example demonstrating how to use the JSON library to serialize and deserialize a configuration for a hypothetical software project:

```cpp
#include <iostream>
#include "json.h"

int main() {
    // Creating a JSON object to represent a project configuration
    JSON config = JSON::Object;
    config["project_name"] = "JSON C++ Library";
    config["version"] = 1.0;
    config["dependencies"] = JSON::Array;
    config["dependencies"].push_back("CMake 3.12 or higher");
    config["dependencies"].push_back("C++ Compiler with C++20 support");

    // Serialize the configuration to a string
    std::string serializedConfig = config.toString();
    std::cout << "Serialized object:\n  " << serializedConfig << std::endl;

    // Deserialize the configuration back into a JSON object
    JSON parsedConfig = JSON::parse(serializedConfig);

    // Pretty print the parsed configuration using the
    // std::format function with the {:p} format specifier
    std::cout << std::format("Parsed object: \n{:p}\n", parsedConfig);

    std::cout << "Check if parsed object is equal to original object: ";
    std::cout << (config == parsedConfig ? "true" : "false") << "\n";

    return 0;
}
```

This example creates a JSON object to store information about project,
then serializes it to a string, and then parses that string back into a JSON object, 
demonstrating the library's basic functionality.

## License

Project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

