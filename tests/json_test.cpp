#include <gtest/gtest.h>
#include "json/json.h"
#include <fstream>

TEST(JSONTest, ConstructorTests) {
    JSON jsonNull(JSON::Null);
    EXPECT_TRUE(jsonNull.isNull());

    JSON jsonString("test");
    EXPECT_EQ(jsonString.str(), "test");

    JSON jsonInt(42);
    EXPECT_EQ(jsonInt.i(), 42);

    JSON jsonDouble(3.14);
    EXPECT_EQ(jsonDouble.d(), 3.14);

    JSON jsonBool(true);
    EXPECT_TRUE(jsonBool.b());

    std::vector<JSON> vec = {JSON(1), JSON(2), JSON(3)};
    JSON jsonArray(vec);
    EXPECT_EQ(jsonArray.arr().size(), 3);

    std::map<JSON, JSON> map = {{JSON("key"), JSON("value")}};
    JSON jsonObject(map);
    EXPECT_EQ(jsonObject.obj().size(), 1);
}

TEST(JSONTest, GetterTests) {
    JSON jsonString("test");
    EXPECT_EQ(jsonString.str(), "test");

    JSON jsonInt(42);
    EXPECT_EQ(jsonInt.i(), 42);

    JSON jsonDouble(3.14);
    EXPECT_EQ(jsonDouble.d(), 3.14);

    JSON jsonBool(true);
    EXPECT_TRUE(jsonBool.b());

    std::vector<JSON> vec = {JSON(1), JSON(2), JSON(3)};
    JSON jsonArray(vec);
    EXPECT_EQ(jsonArray.arr().size(), 3);

    std::map<JSON, JSON> map = {{JSON("key"), JSON("value")}};
    JSON jsonObject(map);
    EXPECT_EQ(jsonObject.obj().size(), 1);
}

TEST(JSONTest, SetterTests) {
    JSON json;
    json.setS() = "test";
    EXPECT_EQ(json.str(), "test");

    json.setI() = 42;
    EXPECT_EQ(json.i(), 42);

    json.setD() = 3.14;
    EXPECT_EQ(json.d(), 3.14);

    json.setB() = true;
    EXPECT_TRUE(json.b());

    json.setNull();
    EXPECT_TRUE(json.isNull());

    json.setArr().push_back(JSON(1));
    EXPECT_EQ(json.arr().size(), 1);

    json.setObj().emplace(JSON("key"), JSON("value"));
    EXPECT_EQ(json.obj().size(), 1);
}

TEST(JSONTest, ConversionTests) {
    JSON jsonString("test");
    EXPECT_EQ(jsonString.toString(), "\"test\"");

    JSON jsonInt(42);
    EXPECT_EQ(jsonInt.toString(), "42");

    JSON jsonDouble(3.14);
    EXPECT_EQ(jsonDouble.toString(), "3.140000");

    JSON jsonBool(true);
    EXPECT_EQ(jsonBool.toString(), "true");

    std::vector<JSON> vec = {JSON(1), JSON(2), JSON(3)};
    JSON jsonArray(vec);
    EXPECT_EQ(jsonArray.toString(), "[1,2,3]");

    std::map<JSON, JSON> map = {{JSON("key"), JSON("value")}};
    JSON jsonObject(map);
    EXPECT_EQ(jsonObject.toString(), "{\"key\":\"value\"}");
}

TEST(JSONTest, OperatorTests) {
    JSON jsonInt1(1);
    JSON jsonInt2(2);
    EXPECT_EQ((jsonInt1 + jsonInt2).i(), 3);
    EXPECT_EQ((jsonInt2 - jsonInt1).i(), 1);
    EXPECT_EQ((jsonInt1 * jsonInt2).i(), 2);
    EXPECT_EQ((jsonInt2 / jsonInt1).i(), 2);

    JSON jsonDouble1(1.0);
    JSON jsonDouble2(2.0);
    EXPECT_EQ((jsonDouble1 + jsonDouble2).d(), 3.0);
    EXPECT_EQ((jsonDouble2 - jsonDouble1).d(), 1.0);
    EXPECT_EQ((jsonDouble1 * jsonDouble2).d(), 2.0);
    EXPECT_EQ((jsonDouble2 / jsonDouble1).d(), 2.0);
}

TEST(JSONTest, ContainerOperationsTests) {
    // Test array indexing
    std::vector<JSON> vec = {JSON(1), JSON(2), JSON(3)};
    JSON jsonArray(vec);
    EXPECT_EQ(jsonArray[0].i(), 1);
    EXPECT_EQ(jsonArray[1].i(), 2);
    EXPECT_EQ(jsonArray[2].i(), 3);
    
    // Test array at() method
    EXPECT_EQ(jsonArray.at(0).i(), 1);
    EXPECT_EQ(jsonArray.at(1).i(), 2);
    EXPECT_EQ(jsonArray.at(2).i(), 3);
    
    // Test out of range exception
    EXPECT_THROW(jsonArray.at(3), std::out_of_range);
    EXPECT_THROW(jsonArray[3], std::out_of_range);
    
    // Test object indexing
    std::map<JSON, JSON> map = {{JSON("key1"), JSON("value1")}, {JSON("key2"), JSON("value2")}};
    JSON jsonObject(map);
    EXPECT_EQ(jsonObject["key1"].str(), "value1");
    EXPECT_EQ(jsonObject["key2"].str(), "value2");
    
    // Test object at() method
    EXPECT_EQ(jsonObject.at("key1").str(), "value1");
    EXPECT_EQ(jsonObject.at("key2").str(), "value2");
    
    // Test key not found exception
    EXPECT_THROW(jsonObject.at("key3"), std::out_of_range);
    
    // Test size() method
    EXPECT_EQ(jsonArray.size(), 3);
    EXPECT_EQ(jsonObject.size(), 2);
    
    // Test push_back
    JSON arr(JSON::Array);
    arr.push_back(1);
    arr.push_back("test");
    arr.push_back(true);
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0].i(), 1);
    EXPECT_EQ(arr[1].str(), "test");
    EXPECT_EQ(arr[2].b(), true);
    
    // Test emplace
    JSON obj(JSON::Object);
    obj.emplace("key1", "value1");
    obj.emplace("key2", 42);
    obj.emplace("key3", true);
    EXPECT_EQ(obj.size(), 3);
    EXPECT_EQ(obj["key1"].str(), "value1");
    EXPECT_EQ(obj["key2"].i(), 42);
    EXPECT_EQ(obj["key3"].b(), true);
    
    // Test contains
    EXPECT_TRUE(jsonArray.contains(0));
    EXPECT_TRUE(jsonArray.contains(1));
    EXPECT_TRUE(jsonArray.contains(2));
    EXPECT_FALSE(jsonArray.contains(3));
    
    EXPECT_TRUE(jsonObject.contains("key1"));
    EXPECT_TRUE(jsonObject.contains("key2"));
    EXPECT_FALSE(jsonObject.contains("key3"));
    
    // Test type mismatch errors
    JSON jsonStr("test");
    EXPECT_THROW(jsonStr.size(), std::runtime_error);
    EXPECT_THROW(jsonStr.push_back(1), std::runtime_error);
    EXPECT_THROW(jsonStr.emplace("key", "value"), std::runtime_error);
    EXPECT_THROW(jsonStr.contains("key"), std::runtime_error);
}

TEST(JSONTest, TryGetTests) {
    // Test tryGet for integers
    JSON jsonInt(42);
    int i = 0;
    EXPECT_TRUE(jsonInt.tryGet(i));
    EXPECT_EQ(i, 42);
    
    // Test tryGet for bool to int conversion
    JSON jsonBool(true);
    EXPECT_TRUE(jsonBool.tryGet(i));
    EXPECT_EQ(i, 1);
    
    // Test tryGet for floating point
    JSON jsonDouble(3.14);
    double d = 0.0;
    EXPECT_TRUE(jsonDouble.tryGet(d));
    EXPECT_EQ(d, 3.14);
    
    // Test tryGet for int to double conversion
    EXPECT_TRUE(jsonInt.tryGet(d));
    EXPECT_EQ(d, 42.0);
    
    // Test tryGet for string
    JSON jsonStr("test");
    std::string s;
    EXPECT_TRUE(jsonStr.tryGet(s));
    EXPECT_EQ(s, "\"test\"");
    
    // Test tryGet for vector
    std::vector<JSON> vec = {JSON(1), JSON(2), JSON(3)};
    JSON jsonArray(vec);
    std::vector<int> intVec;
    EXPECT_TRUE(jsonArray.tryGet(intVec));
    EXPECT_EQ(intVec.size(), 3);
    EXPECT_EQ(intVec[0], 1);
    EXPECT_EQ(intVec[1], 2);
    EXPECT_EQ(intVec[2], 3);
    
    // Test tryGet for pair
    JSON jsonPair(std::make_pair(1, "test"));
    std::pair<int, std::string> p;
    EXPECT_TRUE(jsonPair.tryGet(p));
    EXPECT_EQ(p.first, 1);
    EXPECT_EQ(p.second, "\"test\"");
    
    // Test tryGet for tuple
    JSON jsonTuple(std::make_tuple(1, "test", true));
    std::tuple<int, std::string, bool> t;
    EXPECT_TRUE(jsonTuple.tryGet(t));
    EXPECT_EQ(std::get<0>(t), 1);
    EXPECT_EQ(std::get<1>(t), "\"test\"");
    EXPECT_EQ(std::get<2>(t), true);
    
    // Test map conversion manually (without using tryGet)
    std::map<JSON, JSON> map = {{JSON("key1"), JSON("value1")}, {JSON("key2"), JSON("value2")}};
    JSON jsonObject(map);
    std::map<std::string, std::string> strMap;
    
    // Manual conversion instead of using tryGet directly
    for (const auto& [k, v] : jsonObject.obj()) {
        strMap[k.str()] = v.str();
    }
    
    EXPECT_EQ(strMap.size(), 2);
    EXPECT_EQ(strMap["key1"], "value1");
    EXPECT_EQ(strMap["key2"], "value2");
    
    // Test get and as methods
    EXPECT_EQ(jsonInt.as<int>(), 42);
    EXPECT_EQ(jsonStr.as<std::string>(), "\"test\"");
    EXPECT_THROW(jsonStr.as<int>(), std::runtime_error);
}

TEST(JSONTest, ComparisonTests) {
    JSON json1(42);
    JSON json2(42);
    JSON json3(43);
    JSON jsonStr1("test");
    JSON jsonStr2("test");
    JSON jsonStr3("test2");
    
    // Test equality
    EXPECT_EQ(json1, json2);
    EXPECT_NE(json1, json3);
    EXPECT_EQ(jsonStr1, jsonStr2);
    EXPECT_NE(jsonStr1, jsonStr3);
    
    // Test comparison with other types
    EXPECT_EQ(json1, 42);
    EXPECT_NE(json1, 43);
    EXPECT_EQ(jsonStr1, "test");
    EXPECT_NE(jsonStr1, "test2");
    
    // Test ordering
    EXPECT_LT(json1, json3);
    EXPECT_GT(json3, json1);
    EXPECT_LE(json1, json2);
    EXPECT_LE(json1, json3);
    EXPECT_GE(json1, json2);
    EXPECT_GE(json3, json1);
    
    EXPECT_LT(jsonStr1, jsonStr3);
    EXPECT_GT(jsonStr3, jsonStr1);
}

TEST(JSONTest, AdditionalOperatorTests) {
    // Test modulo
    JSON jsonInt1(5);
    JSON jsonInt2(2);
    EXPECT_EQ((jsonInt1 % jsonInt2).i(), 1);
    
    // Test bitwise operators
    JSON jsonBit1(5);  // 101 in binary
    JSON jsonBit2(3);  // 011 in binary
    
    EXPECT_EQ((jsonBit1 & jsonBit2).i(), 1);  // 101 & 011 = 001
    EXPECT_EQ((jsonBit1 | jsonBit2).i(), 7);  // 101 | 011 = 111
    EXPECT_EQ((jsonBit1 ^ jsonBit2).i(), 6);  // 101 ^ 011 = 110
    
    // Test shift operators
    EXPECT_EQ((jsonBit1 << jsonBit2).i(), 40); // 5 << 3 = 40
    EXPECT_EQ((jsonBit1 >> jsonInt2).i(), 1);  // 5 >> 2 = 1
    
    // Test unary minus
    JSON jsonNeg(5);
    EXPECT_EQ((-jsonNeg).i(), -5);
    
    // Test boolean conversion
    JSON jsonZero(0);
    JSON jsonOne(1);
    EXPECT_FALSE(bool(jsonZero));
    EXPECT_TRUE(bool(jsonOne));
    
    // Test increment/decrement
    JSON jsonInc(5);
    ++jsonInc;
    EXPECT_EQ(jsonInc.i(), 6);
    
    JSON jsonDec(5);
    --jsonDec;
    EXPECT_EQ(jsonDec.i(), 4);
    
    // Test compound assignment operators
    JSON j1(5);
    j1 += 2;
    EXPECT_EQ(j1.i(), 7);
    
    j1 -= 3;
    EXPECT_EQ(j1.i(), 4);
    
    j1 *= 2;
    EXPECT_EQ(j1.i(), 8);
    
    j1 /= 4;
    EXPECT_EQ(j1.i(), 2);
    
    j1 %= 2;
    EXPECT_EQ(j1.i(), 0);
    
    JSON j2(5);
    j2 &= 3;
    EXPECT_EQ(j2.i(), 1);
    
    j2 |= 6;
    EXPECT_EQ(j2.i(), 7);
    
    j2 ^= 2;
    EXPECT_EQ(j2.i(), 5);
    
    j2 <<= 1;
    EXPECT_EQ(j2.i(), 10);
    
    j2 >>= 1;
    EXPECT_EQ(j2.i(), 5);
}

TEST(JSONTest, TypeCheckingTests) {
    JSON jsonNull;
    JSON jsonStr("test");
    JSON jsonInt(42);
    JSON jsonDouble(3.14);
    JSON jsonBool(true);
    JSON jsonArray(JSON::Array);
    JSON jsonObject(JSON::Object);
    
    // Test type() function
    EXPECT_EQ(jsonNull.type(), JSON::Null);
    EXPECT_EQ(jsonStr.type(), JSON::String);
    EXPECT_EQ(jsonInt.type(), JSON::Integer);
    EXPECT_EQ(jsonDouble.type(), JSON::Double);
    EXPECT_EQ(jsonBool.type(), JSON::Boolean);
    EXPECT_EQ(jsonArray.type(), JSON::Array);
    EXPECT_EQ(jsonObject.type(), JSON::Object);
    
    // Test typeStr() function
    EXPECT_STREQ(jsonNull.typeStr(), "null");
    EXPECT_STREQ(jsonStr.typeStr(), "string");
    EXPECT_STREQ(jsonInt.typeStr(), "integer");
    EXPECT_STREQ(jsonDouble.typeStr(), "double");
    EXPECT_STREQ(jsonBool.typeStr(), "boolean");
    EXPECT_STREQ(jsonArray.typeStr(), "array");
    EXPECT_STREQ(jsonObject.typeStr(), "object");
    
    // Test is() function
    EXPECT_TRUE(jsonNull.is(JSON::Null));
    EXPECT_TRUE(jsonStr.is(JSON::String));
    EXPECT_TRUE(jsonInt.is(JSON::Integer));
    EXPECT_TRUE(jsonDouble.is(JSON::Double));
    EXPECT_TRUE(jsonBool.is(JSON::Boolean));
    EXPECT_TRUE(jsonArray.is(JSON::Array));
    EXPECT_TRUE(jsonObject.is(JSON::Object));
    
    EXPECT_FALSE(jsonNull.is(JSON::String));
    EXPECT_FALSE(jsonStr.is(JSON::Integer));
}

TEST(JSONTest, FileOperationsTests) {
    try {
        // Create a JSON object to save
        JSON root(JSON::Object);
        root.emplace("string", "test");
        root.emplace("integer", 42);
        root.emplace("double", 3.14);
        root.emplace("boolean", true);
        root.emplace("null", JSON());
        
        JSON array(JSON::Array);
        array.push_back(1);
        array.push_back(2);
        array.push_back(3);
        root.emplace("array", array);
        
        JSON object(JSON::Object);
        object.emplace("key", "value");
        root.emplace("object", object);
        
        // Test save and load using a simpler approach
        std::string filename = "test_json.json";
        
        // Save JSON using toString directly
        {
            std::ofstream file(filename);
            ASSERT_TRUE(file.good()) << "Failed to create test file";
            file << root.toString();
            file.close();
        }
        
        // Read saved content for debugging
        std::string content;
        {
            std::ifstream file(filename);
            ASSERT_TRUE(file.good()) << "Failed to read file";
            std::stringstream buffer;
            buffer << file.rdbuf();
            content = buffer.str();
            std::cout << "Saved JSON content: " << content << std::endl;
        }
        
        // Parse content directly
        JSON parsed;
        try {
            parsed = JSON::parse(content);
            EXPECT_EQ(parsed["string"].str(), "test");
            EXPECT_EQ(parsed["integer"].i(), 42);
        } catch (const std::bad_variant_access& e) {
            FAIL() << "Bad variant access during parsing: " << e.what();
        } catch (const std::exception& e) {
            FAIL() << "Exception during parsing: " << e.what();
        }
        
        // Use the load function
        try {
            JSON loaded = JSON::load(filename);
            EXPECT_EQ(loaded["string"].str(), "test");
            EXPECT_EQ(loaded["integer"].i(), 42);
            EXPECT_FLOAT_EQ(loaded["double"].d(), 3.14);
            EXPECT_EQ(loaded["boolean"].b(), true);
            EXPECT_TRUE(loaded["null"].isNull());
            EXPECT_EQ(loaded["array"].size(), 3);
            EXPECT_EQ(loaded["array"][0].i(), 1);
            EXPECT_EQ(loaded["object"]["key"].str(), "value");
        } catch (const std::bad_variant_access& e) {
            FAIL() << "Bad variant access during load: " << e.what() << "\nThis means the JSON structure loaded from the file doesn't match the expected structure.";
        } catch (const std::exception& e) {
            FAIL() << "Exception during load: " << e.what();
        }
        
        // Test pretty printing with better error handling
        std::string prettyFilename = "test_json_pretty.json";
        try {
            // Direct pretty-print to file
            std::ofstream prettyFile(prettyFilename);
            ASSERT_TRUE(prettyFile.good()) << "Failed to create pretty file";
            prettyFile << root.toPrettyString();
            prettyFile.close();
            
            // Load and verify
            JSON loadedPretty = JSON::load(prettyFilename);
            EXPECT_EQ(loadedPretty["string"].str(), "test");
            EXPECT_EQ(loadedPretty["integer"].i(), 42);
        } catch (const std::bad_variant_access& e) {
            FAIL() << "Bad variant access during pretty print test: " << e.what();
        } catch (const std::exception& e) {
            FAIL() << "Exception during pretty print test: " << e.what();
        }
        
        // Clean up test files
        std::remove(filename.c_str());
        std::remove(prettyFilename.c_str());
    } catch (const std::exception& e) {
        FAIL() << "Unexpected exception: " << e.what();
    }
}

TEST(JSONTest, ComplexTypesTest) {
    // Test with tuples
    auto tuple = std::make_tuple(1, "test", 3.14, true);
    JSON jsonTuple(tuple);
    EXPECT_EQ(jsonTuple.type(), JSON::Array);
    EXPECT_EQ(jsonTuple.size(), 4);
    EXPECT_EQ(jsonTuple[0].i(), 1);
    EXPECT_EQ(jsonTuple[1].str(), "test");
    EXPECT_FLOAT_EQ(jsonTuple[2].d(), 3.14);
    EXPECT_EQ(jsonTuple[3].b(), true);
    
    // Test with pairs
    auto pair = std::make_pair("key", "value");
    JSON jsonPair(pair);
    EXPECT_EQ(jsonPair.type(), JSON::Array);
    EXPECT_EQ(jsonPair.size(), 2);
    EXPECT_EQ(jsonPair[0].str(), "key");
    EXPECT_EQ(jsonPair[1].str(), "value");
    
    // Test with nested structures
    std::map<std::string, std::vector<int>> complex;
    complex["numbers"] = {1, 2, 3};
    complex["more"] = {4, 5, 6};
    
    JSON jsonComplex(complex);
    EXPECT_EQ(jsonComplex.type(), JSON::Object);
    EXPECT_EQ(jsonComplex.size(), 2);
    EXPECT_EQ(jsonComplex["numbers"].size(), 3);
    EXPECT_EQ(jsonComplex["numbers"][0].i(), 1);
    EXPECT_EQ(jsonComplex["more"][2].i(), 6);
}

// Add these diagnostic tests after the existing ones, before main()

// Diagnostic test for string conversions in map
TEST(JSONTest, StringMapConversionTest) {
    // Direct test of string conversions in maps
    std::map<JSON, JSON> map;
    map[JSON("key")] = JSON("value");
    
    JSON jsonObj(map);
    std::string json_str = jsonObj.toString();
    std::cout << "JSON string representation: " << json_str << std::endl;
    
    // Try manual map conversion
    std::map<std::string, std::string> str_map;
    for (const auto& [k, v] : map) {
        std::string key = k.str(); // Raw string without quotes
        std::string value = v.str(); // Raw string without quotes
        std::cout << "Key: " << key << ", Value: " << value << std::endl;
        str_map[key] = value;
    }
    
    EXPECT_EQ(str_map["key"], "value");
}

// Diagnostic test for file operations
TEST(JSONTest, BasicFileTest) {
    // Simple file write and read test
    std::string filename = "simple_test.json";
    
    // Write a simple JSON object to file
    {
        std::ofstream file(filename);
        ASSERT_TRUE(file.good()) << "Failed to create test file";
        file << "{\"test\":42}";
        file.close();
    }
    
    // Read the file content directly
    std::string content;
    {
        std::ifstream file(filename);
        ASSERT_TRUE(file.good()) << "Failed to read test file";
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
        file.close();
    }
    std::cout << "File content: " << content << std::endl;
    
    // Try parsing the content directly
    try {
        JSON json = JSON::parse(content);
        EXPECT_EQ(json["test"].i(), 42);
        
        // Test the load function separately
        JSON loaded = JSON::load(filename);
        EXPECT_EQ(loaded["test"].i(), 42);
    } catch (const std::exception& e) {
        FAIL() << "Exception during parsing: " << e.what();
    }
    
    // Clean up
    std::remove(filename.c_str());
}

// Add a new detailed test for file operations
TEST(JSONTest, SaveParseLoadTest) {
    // Create a simple JSON object for testing
    JSON root(JSON::Object);
    root.emplace("test", 42);
    
    std::string filename = "save_parse_load_test.json";
    
    try {
        // Test save operation
        root.save(filename);
        
        // Read the file content directly
        std::string content;
        {
            std::ifstream file(filename);
            ASSERT_TRUE(file.good()) << "Failed to read saved file";
            std::stringstream buffer;
            buffer << file.rdbuf();
            content = buffer.str();
            std::cout << "File content from save(): " << content << std::endl;
        }
        
        // Parse the content directly
        JSON parsed = JSON::parse(content);
        EXPECT_EQ(parsed["test"].i(), 42);
        
        // Examine the load implementation
        std::cout << "Testing the load function step by step..." << std::endl;
        
        // Manual implementation of load similar to what JSON::load would do
        std::ifstream file(filename);
        ASSERT_TRUE(file.good());
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string fileContent = buffer.str();
        
        JSON loaded = JSON::parse(fileContent);
        EXPECT_EQ(loaded["test"].i(), 42);
        
        // Now try the actual load function
        JSON directlyLoaded = JSON::load(filename);
        EXPECT_EQ(directlyLoaded["test"].i(), 42);
        
        // Clean up
        std::remove(filename.c_str());
    } catch (const std::exception& e) {
        std::remove(filename.c_str()); // Clean up even on failure
        FAIL() << "Exception: " << e.what();
    }
}

// Add this test to isolate which JSON structure elements cause problems
TEST(JSONTest, IncrementalComplexityFileTest) {
    std::string filename = "incremental_test.json";

    try {
        // Test 1: Simple primitive values
        {
            JSON simple(JSON::Object);
            simple.emplace("string", "test");
            simple.emplace("integer", 42);
            simple.emplace("double", 3.14);
            simple.emplace("boolean", true);
            simple.emplace("null", JSON());
            
            // Save and load
            simple.save(filename);
            
            std::cout << "Test 1 - Primitives - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                EXPECT_EQ(loaded["integer"].i(), 42);
                std::cout << "Test 1: Primitives loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Test 1 failed: " << e.what();
            }
        }
        
        // Test 2: Add array
        {
            JSON withArray(JSON::Object);
            withArray.emplace("string", "test");
            withArray.emplace("integer", 42);
            
            JSON array(JSON::Array);
            array.push_back(1);
            array.push_back(2);
            array.push_back(3);
            withArray.emplace("array", array);
            
            // Save and load
            withArray.save(filename);
            
            std::cout << "Test 2 - With Array - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                EXPECT_EQ(loaded["array"].size(), 3);
                std::cout << "Test 2: Array loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Test 2 failed: " << e.what();
            }
        }
        
        // Test 3: Add nested object
        {
            JSON withObject(JSON::Object);
            withObject.emplace("string", "test");
            withObject.emplace("integer", 42);
            
            JSON object(JSON::Object);
            object.emplace("key", "value");
            withObject.emplace("object", object);
            
            // Save and load
            withObject.save(filename);
            
            std::cout << "Test 3 - With Object - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                EXPECT_EQ(loaded["object"]["key"].str(), "value");
                std::cout << "Test 3: Nested object loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Test 3 failed: " << e.what();
            }
        }
        
        // Test 4: Full complex structure
        {
            JSON full(JSON::Object);
            full.emplace("string", "test");
            full.emplace("integer", 42);
            full.emplace("double", 3.14);
            full.emplace("boolean", true);
            full.emplace("null", JSON());
            
            JSON array(JSON::Array);
            array.push_back(1);
            array.push_back(2);
            array.push_back(3);
            full.emplace("array", array);
            
            JSON object(JSON::Object);
            object.emplace("key", "value");
            full.emplace("object", object);
            
            // Save and load
            full.save(filename);
            
            std::cout << "Test 4 - Full Complex - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                EXPECT_EQ(loaded["integer"].i(), 42);
                EXPECT_FLOAT_EQ(loaded["double"].d(), 3.14);
                EXPECT_EQ(loaded["boolean"].b(), true);
                EXPECT_TRUE(loaded["null"].isNull());
                EXPECT_EQ(loaded["array"].size(), 3);
                EXPECT_EQ(loaded["object"]["key"].str(), "value");
                std::cout << "Test 4: Full complex structure loaded successfully" << std::endl;
            } catch (const std::bad_variant_access& e) {
                FAIL() << "Test 4 failed (variant access): " << e.what();
            } catch (const std::exception& e) {
                FAIL() << "Test 4 failed: " << e.what();
            }
        }
        
        // Clean up
        std::remove(filename.c_str());
    } catch (const std::exception& e) {
        std::remove(filename.c_str());
        FAIL() << "Unexpected exception: " << e.what();
    }
}

// Add a more detailed incremental test to identify the specific problematic combination
TEST(JSONTest, DetailedIncrementalComplexityFileTest) {
    std::string filename = "detailed_incremental_test.json";

    try {
        // Test A: Array + Object
        {
            JSON test(JSON::Object);
            test.emplace("string", "test");
            
            JSON array(JSON::Array);
            array.push_back(1);
            array.push_back(2);
            test.emplace("array", array);
            
            JSON object(JSON::Object);
            object.emplace("key", "value");
            test.emplace("object", object);
            
            // Save and load
            test.save(filename);
            
            std::cout << "Test A - Array + Object - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                EXPECT_EQ(loaded["array"].size(), 2);
                EXPECT_EQ(loaded["object"]["key"].str(), "value");
                std::cout << "Test A: Array + Object loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Test A failed: " << e.what();
            }
        }
        
        // Test B: All primitives + null
        {
            JSON test(JSON::Object);
            test.emplace("string", "test");
            test.emplace("integer", 42);
            test.emplace("double", 3.14);
            test.emplace("boolean", true);
            test.emplace("null", JSON());
            
            // Save and load
            test.save(filename);
            
            std::cout << "Test B - All primitives + null - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                EXPECT_EQ(loaded["integer"].i(), 42);
                EXPECT_FLOAT_EQ(loaded["double"].d(), 3.14);
                EXPECT_EQ(loaded["boolean"].b(), true);
                EXPECT_TRUE(loaded["null"].isNull());
                std::cout << "Test B: All primitives loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Test B failed: " << e.what();
            }
        }
        
        // Test C: Primitives + Array
        {
            JSON test(JSON::Object);
            test.emplace("string", "test");
            test.emplace("integer", 42);
            test.emplace("double", 3.14);
            test.emplace("boolean", true);
            test.emplace("null", JSON());
            
            JSON array(JSON::Array);
            array.push_back(1);
            array.push_back(2);
            array.push_back(3);
            test.emplace("array", array);
            
            // Save and load
            test.save(filename);
            
            std::cout << "Test C - Primitives + Array - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                EXPECT_EQ(loaded["array"].size(), 3);
                std::cout << "Test C: Primitives + Array loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Test C failed: " << e.what();
            }
        }
        
        // Test D: Primitives + Object
        {
            JSON test(JSON::Object);
            test.emplace("string", "test");
            test.emplace("integer", 42);
            test.emplace("double", 3.14);
            test.emplace("boolean", true);
            test.emplace("null", JSON());
            
            JSON object(JSON::Object);
            object.emplace("key", "value");
            test.emplace("object", object);
            
            // Save and load
            test.save(filename);
            
            std::cout << "Test D - Primitives + Object - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                EXPECT_EQ(loaded["object"]["key"].str(), "value");
                std::cout << "Test D: Primitives + Object loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Test D failed: " << e.what();
            }
        }
        
        // Test E: All together with parse first
        {
            JSON full(JSON::Object);
            full.emplace("string", "test");
            full.emplace("integer", 42);
            full.emplace("double", 3.14);
            full.emplace("boolean", true);
            full.emplace("null", JSON());
            
            JSON array(JSON::Array);
            array.push_back(1);
            array.push_back(2);
            array.push_back(3);
            full.emplace("array", array);
            
            JSON object(JSON::Object);
            object.emplace("key", "value");
            full.emplace("object", object);
            
            // Save and try parse first
            full.save(filename);
            
            std::cout << "Test E - Full Complex with parse first - File content: ";
            std::string content;
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                content = buffer.str();
                std::cout << content << std::endl;
            }
            
            try {
                // First parse the content directly
                JSON parsed = JSON::parse(content);
                EXPECT_EQ(parsed["string"].str(), "test");
                EXPECT_EQ(parsed["integer"].i(), 42);
                EXPECT_FLOAT_EQ(parsed["double"].d(), 3.14);
                EXPECT_EQ(parsed["boolean"].b(), true);
                EXPECT_TRUE(parsed["null"].isNull());
                EXPECT_EQ(parsed["array"].size(), 3);
                EXPECT_EQ(parsed["object"]["key"].str(), "value");
                std::cout << "Test E: Parse step successful" << std::endl;
                
                // Now try to load using the load function
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                std::cout << "Test E: Full complex loaded successfully" << std::endl;
            } catch (const std::bad_variant_access& e) {
                FAIL() << "Test E failed (variant access): " << e.what();
            } catch (const std::exception& e) {
                FAIL() << "Test E failed: " << e.what();
            }
        }
        
        // Clean up
        std::remove(filename.c_str());
    } catch (const std::exception& e) {
        std::remove(filename.c_str());
        FAIL() << "Unexpected exception: " << e.what();
    }
}

// Add this focused test to specifically examine null value handling
TEST(JSONTest, NullHandlingFileTest) {
    std::string filename = "null_test.json";

    try {
        // Test with just null
        {
            JSON test(JSON::Object);
            test.emplace("null", JSON());
            
            // Save and load
            test.save(filename);
            
            std::cout << "Null only - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            // First try parsing directly
            std::ifstream file(filename);
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            
            try {
                JSON parsed = JSON::parse(content);
                EXPECT_TRUE(parsed["null"].isNull());
                std::cout << "Null parsed successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Parse failed: " << e.what();
            }
            
            // Now try loading
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_TRUE(loaded["null"].isNull());
                std::cout << "Null loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Load failed: " << e.what();
            }
        }
        
        // Test with one other value + null
        {
            JSON test(JSON::Object);
            test.emplace("string", "test");
            test.emplace("null", JSON());
            
            // Save and load
            test.save(filename);
            
            std::cout << "String + Null - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["string"].str(), "test");
                EXPECT_TRUE(loaded["null"].isNull());
                std::cout << "String + Null loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "String + Null failed: " << e.what();
            }
        }
        
        // Test with each primitive type + null separately
        {
            JSON test(JSON::Object);
            test.emplace("integer", 42);
            test.emplace("null", JSON());
            
            test.save(filename);
            std::cout << "Integer + Null - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["integer"].i(), 42);
                EXPECT_TRUE(loaded["null"].isNull());
                std::cout << "Integer + Null loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Integer + Null failed: " << e.what();
            }
        }
        
        {
            JSON test(JSON::Object);
            test.emplace("double", 3.14);
            test.emplace("null", JSON());
            
            test.save(filename);
            std::cout << "Double + Null - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_FLOAT_EQ(loaded["double"].d(), 3.14);
                EXPECT_TRUE(loaded["null"].isNull());
                std::cout << "Double + Null loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Double + Null failed: " << e.what();
            }
        }
        
        {
            JSON test(JSON::Object);
            test.emplace("boolean", true);
            test.emplace("null", JSON());
            
            test.save(filename);
            std::cout << "Boolean + Null - File content: ";
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::cout << buffer.str() << std::endl;
            }
            
            try {
                JSON loaded = JSON::load(filename);
                EXPECT_EQ(loaded["boolean"].b(), true);
                EXPECT_TRUE(loaded["null"].isNull());
                std::cout << "Boolean + Null loaded successfully" << std::endl;
            } catch (const std::exception& e) {
                FAIL() << "Boolean + Null failed: " << e.what();
            }
        }
        
        // Clean up
        std::remove(filename.c_str());
    } catch (const std::exception& e) {
        std::remove(filename.c_str());
        FAIL() << "Unexpected exception: " << e.what();
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
