#include <gtest/gtest.h>
#include "json/json.h"

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
