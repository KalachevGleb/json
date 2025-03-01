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
