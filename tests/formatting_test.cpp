#include <gtest/gtest.h>
#include "../json/json.h"
#include <string>

TEST(JSONFormattingTest, ArrayFormattingWithLineWrapping) {
    // Создаем массив для тестирования форматирования с переносами строк
    JSON array(JSON::Array); // Используем enum, а не вызов функции
    array.arr().push_back(1);
    array.arr().push_back(2);
    array.arr().push_back(3);
    
    // Проверяем форматирование с переносом строк
    std::string result;
    array.toString(result, true); // wrapLines = true
    
    // Получаем фактический вывод из исполнения
    // Проверяем лишь что результат содержит необходимые элементы с переносом строк
    EXPECT_NE(result.find("[\n"), std::string::npos);
    EXPECT_NE(result.find("1,"), std::string::npos);
    EXPECT_NE(result.find("2,"), std::string::npos);
    EXPECT_NE(result.find("3"), std::string::npos);
    EXPECT_NE(result.find("]"), std::string::npos);
}

TEST(JSONFormattingTest, ObjectFormattingWithLineWrapping) {
    // Создаем объект для тестирования форматирования с переносами строк
    JSON obj(JSON::Object); // Используем enum, а не вызов функции
    obj["key1"] = 1;
    obj["key2"] = "value";
    obj["key3"] = true;
    
    // Проверяем форматирование с переносом строк
    std::string result;
    obj.toString(result, true); // wrapLines = true
    
    // Проверяем лишь что результат содержит необходимые элементы с переносом строк
    EXPECT_NE(result.find("{\n"), std::string::npos);
    EXPECT_NE(result.find("\"key1\""), std::string::npos);
    EXPECT_NE(result.find("\"key2\""), std::string::npos); 
    EXPECT_NE(result.find("\"key3\""), std::string::npos);
    EXPECT_NE(result.find("1"), std::string::npos);
    EXPECT_NE(result.find("\"value\""), std::string::npos);
    EXPECT_NE(result.find("true"), std::string::npos);
    EXPECT_NE(result.find("}"), std::string::npos);
}

TEST(JSONFormattingTest, PrettyFormattingWithWrapping) {
    // Тестируем функцию toPrettyString с параметрами для оптимального форматирования
    JSON complex(JSON::Object); // Используем enum, а не вызов функции
    complex["array"] = JSON(JSON::Array);
    complex["array"].arr().push_back(1);
    complex["array"].arr().push_back(2);
    
    complex["object"] = JSON(JSON::Object);
    complex["object"]["nested"] = "value";
    
    // Вызываем toPrettyString с различными параметрами
    // maxLineLength=20, minWrapLen=10, indent=2
    std::string prettyResult = complex.toPrettyString(20, 10, 2);
    
    // Проверяем, что результат содержит переносы строк
    EXPECT_NE(prettyResult.find('\n'), std::string::npos);
}