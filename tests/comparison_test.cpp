#include <gtest/gtest.h>
#include "../json/json.h"
#include <string>

TEST(JSONComparisonTest, CompareArrays) {
    // Тестируем сравнение массивов разной длины
    JSON arr1(JSON::Array);
    arr1.arr().push_back(1);
    arr1.arr().push_back(2);
    
    JSON arr2(JSON::Array);
    arr2.arr().push_back(1);
    arr2.arr().push_back(2);
    arr2.arr().push_back(3);
    
    // Разной длины
    EXPECT_LT(arr1, arr2);
    EXPECT_GT(arr2, arr1);
    
    // Одинаковой длины, но разные элементы
    JSON arr3(JSON::Array);
    arr3.arr().push_back(1);
    arr3.arr().push_back(3); // Значение больше чем в arr1
    
    EXPECT_LT(arr1, arr3);
    EXPECT_GT(arr3, arr1);
    
    // Равные массивы
    JSON arr4(JSON::Array);
    arr4.arr().push_back(1);
    arr4.arr().push_back(2);
    
    EXPECT_EQ(arr1, arr4);
}

TEST(JSONComparisonTest, CompareObjects) {
    // Тестируем сравнение объектов
    JSON obj1(JSON::Object);
    obj1["a"] = 1;
    obj1["b"] = 2;
    
    JSON obj2(JSON::Object);
    obj2["a"] = 1;
    obj2["b"] = 2;
    obj2["c"] = 3;
    
    // Разной длины
    EXPECT_LT(obj1, obj2);
    EXPECT_GT(obj2, obj1);
    
    // Одинаковой длины, но разные значения
    JSON obj3(JSON::Object);
    obj3["a"] = 1;
    obj3["b"] = 3; // Значение больше чем в obj1
    
    EXPECT_LT(obj1, obj3);
    EXPECT_GT(obj3, obj1);
    
    // Равные объекты
    JSON obj4(JSON::Object);
    obj4["a"] = 1;
    obj4["b"] = 2;
    
    EXPECT_EQ(obj1, obj4);
}

TEST(JSONComparisonTest, CompareDifferentTypes) {
    // Тестируем сравнение разных типов
    // Проверяем фактический порядок сравнения типов 
    
    JSON null;
    JSON boolVal = true;
    JSON intVal = 42;
    JSON doubleVal = 3.14;
    JSON strVal = "test";
    JSON arrVal(JSON::Array);
    JSON objVal(JSON::Object);
    
    // Определяем фактический порядок сравнения типов
    // Дополнительно тестируем операторы >= и <=
    EXPECT_LE(null, null);
    EXPECT_GE(null, null);
    
    // Проверяем, что null наименьший
    EXPECT_LE(null, boolVal);
    EXPECT_LE(null, intVal);
    EXPECT_LE(null, doubleVal);
    EXPECT_LE(null, strVal);
    EXPECT_LE(null, arrVal);
    EXPECT_LE(null, objVal);
    
    // Сравниваем строки с другими типами
    EXPECT_LE(strVal, arrVal);
    EXPECT_LE(strVal, objVal);
    
    // Сравниваем массивы и объекты
    EXPECT_LE(arrVal, objVal);
}

TEST(JSONComparisonTest, ComparePrimitiveTypes) {
    // Тестируем сравнения примитивных типов между собой
    
    // Boolean
    JSON bTrue = true;
    JSON bFalse = false;
    EXPECT_LT(bFalse, bTrue);
    EXPECT_GT(bTrue, bFalse);
    
    // Integer
    JSON i1 = 10;
    JSON i2 = 20;
    EXPECT_LT(i1, i2);
    EXPECT_GT(i2, i1);
    
    // Double
    JSON d1 = 1.5;
    JSON d2 = 2.5;
    EXPECT_LT(d1, d2);
    EXPECT_GT(d2, d1);
    
    // String
    JSON s1 = "abc";
    JSON s2 = "def";
    EXPECT_LT(s1, s2);
    EXPECT_GT(s2, s1);
}