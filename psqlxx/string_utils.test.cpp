#include <psqlxx/string_utils.hpp>

#include <gtest/gtest.h>


using namespace psqlxx;

const char *PREFIX = "a_prefix";

TEST(StartsWithTests, ReturnFalseIfGiveEmptyString) {
    ASSERT_FALSE(StartsWith("", PREFIX));
}
