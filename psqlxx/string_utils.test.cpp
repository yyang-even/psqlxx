#include <psqlxx/string_utils.hpp>

#include <gtest/gtest.h>


using namespace psqlxx;
using namespace std::string_literals;


const auto PREFIX = "a_prefix"s;


TEST(StartsWithTests, ReturnFalseIfGiveEmptyString) {
    ASSERT_FALSE(StartsWith("", PREFIX));
}

TEST(StartsWithTests, ReturnTrueIfGiveEmptyPrefix) {
    ASSERT_TRUE(StartsWith(PREFIX, ""));
}

TEST(StartsWithTests, ReturnTrueIfGiveNullPrefix) {
    ASSERT_TRUE(StartsWith(PREFIX, {}));
}

TEST(StartsWithTests, ReturnTrueIfGiveRightPrefix) {
    ASSERT_TRUE(StartsWith(PREFIX, PREFIX));
}

TEST(StartsWithTests, ReturnFalseIfGiveWrongPrefix) {
    ASSERT_FALSE(StartsWith("some_string"s + PREFIX, PREFIX));
}


TEST(SpaceJoinerTests, ReturnExpectedSpaces) {
    ASSERT_EQ(std::string::npos, PREFIX.find(' '));
    const auto result = SpaceJoiner(PREFIX, PREFIX);
    ASSERT_EQ(1, std::count(result.cbegin(), result.cend(), ' '));
}

TEST(SpaceJoinerTests, CanJoinTypes) {
    ASSERT_EQ(std::string::npos, PREFIX.find(' '));
    const auto result = SpaceJoiner(1, PREFIX, 2.4);
    ASSERT_EQ(2, std::count(result.cbegin(), result.cend(), ' '));
}
