#include <gtest/gtest.h>
#include "../json/json.h"
#include <string>

TEST(JSONStringEscapeTest, EscapedQuotesAndBackslashes) {
    // Тестируем экранирование кавычек и обратных слешей
    JSON jsonWithQuotes = std::string("Hello \"world\"");
    std::string result;
    jsonWithQuotes.toString(result);
    EXPECT_EQ(result, "\"Hello \\\"world\\\"\"");
    
    JSON jsonWithBackslash = std::string("C:\\Program Files\\App");
    result.clear();
    jsonWithBackslash.toString(result);
    EXPECT_EQ(result, "\"C:\\\\Program Files\\\\App\"");
    
    // Проверяем toPrettyString
    std::string prettyResult = jsonWithQuotes.toPrettyString();
    EXPECT_EQ(prettyResult, "\"Hello \\\"world\\\"\"");
}

TEST(JSONStringEscapeTest, ParseEscapedSequences) {
    // Тестируем парсинг различных экранированных последовательностей
    const char* jsonStr = "\"Hello \\\"world\\\" with \\\\ backslash and \\/\\b\\f\\n\\r\\t symbols\"";
    JSON parsed = JSON::parse(jsonStr);
    
    EXPECT_EQ(parsed.type(), JSON::Type::String);
    EXPECT_EQ(parsed.str(), "Hello \"world\" with \\ backslash and /\b\f\n\r\t symbols");
    
    // Проверяем Unicode последовательности
    const char* unicodeStr = "\"Unicode: \\u0041\\u0042\\u0043\"";
    JSON unicodeParsed = JSON::parse(unicodeStr);
    
    EXPECT_EQ(unicodeParsed.type(), JSON::Type::String);
    EXPECT_EQ(unicodeParsed.str(), "Unicode: ABC"); // ASCII символы через Unicode escape
}