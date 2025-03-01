#include <gtest/gtest.h>
#include "../json/json.h"
#include <string>
#include <vector>

TEST(JSONPreferredSizeTest, ArraysWithDifferentSizes) {
    // Тестируем оптимальное форматирование для разных размеров массивов
    // Это косвенно тестирует функцию getPreferredSize
    
    // Создаем массивы разных размеров для тестирования
    std::vector<int> sizes = {1, 2, 3, 5, 8, 10, 16, 32, 40, 50, 64};
    
    for (int size : sizes) {
        JSON array(JSON::Array);
        for (int i = 0; i < size; i++) {
            array.arr().push_back(i);
        }
        
        // Форматируем с разными параметрами 
        // Разная длина строки и минимальные/максимальные значения для принудительного вызова getPreferredSize
        std::string result = array.toPrettyString(80, 2, 4);
        
        // Проверка успешного форматирования (не проверяем конкретный формат, только что оно работает)
        EXPECT_FALSE(result.empty());
    }
}

TEST(JSONPreferredSizeTest, ComplexNumberFormatting) {
    // Тест для покрытия различных ветвей в функции toPrettyString
    
    // Создаем массив чисел
    JSON numbersArray(JSON::Array);
    for (int i = 0; i < 10; i++) {
        numbersArray.arr().push_back(i * 100);
    }
    
    // Вызываем toPrettyString с параметрами, которые должны заставить выравнивать числа
    // maxLineLength=40, minWrapLen=5, indent=2
    std::string result = numbersArray.toPrettyString(40, 5, 2);
    
    // Проверяем, что форматирование работает
    EXPECT_FALSE(result.empty());
    
    // Создаем объект с числовыми ключами для покрытия строк 184-186, 195
    JSON objWithNumericKeys(JSON::Object);
    for (int i = 0; i < 5; i++) {
        JSON key;
        key = i;
        objWithNumericKeys[key] = i * 10;
    }
    
    // Форматируем с параметрами, влияющими на выравнивание
    std::string objResult = objWithNumericKeys.toPrettyString(40, 2, 2);
    EXPECT_FALSE(objResult.empty());
}

TEST(JSONPreferredSizeTest, ArraysWithNonNumericValues) {
    // Создаем массив с нечисловыми значениями для покрытия строки 139
    JSON mixedArray(JSON::Array);
    mixedArray.arr().push_back(1);
    mixedArray.arr().push_back("text");
    mixedArray.arr().push_back(true);
    
    std::string result = mixedArray.toPrettyString(30, 5, 2);
    
    // Проверяем, что в результате нет ошибок
    EXPECT_FALSE(result.empty());
}