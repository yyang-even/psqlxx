#include <psqlxx/command.hpp>

#include <gtest/gtest.h>


using namespace psqlxx;


TEST(ValidCommandTests, ReturnFalseIfNoAction) {
    ASSERT_FALSE(internal::validCommand({}, {}, {}));
}

TEST(ValidCommandTests, ReturnTrueIfNoNameAndNoArguments) {
    ASSERT_TRUE(internal::validCommand({}, {}, &Quit));
}

TEST(ValidCommandTests, ReturnTrueIfGivenEmptyName) {
    ASSERT_TRUE(internal::validCommand({""}, {}, &Quit));
}

TEST(ValidCommandTests, ReturnFalseIfGivenEmptyArgument) {
    ASSERT_FALSE(internal::validCommand({}, {""}, &Quit));
}

TEST(ValidCommandTests, ReturnFalseIfNameContainsSpace) {
    ASSERT_FALSE(internal::validCommand({" "}, {}, &Quit));
}

TEST(ValidCommandTests, ReturnFalseIfArgumentContainsSpace) {
    ASSERT_FALSE(internal::validCommand({}, {" "}, &Quit));
}

TEST(ValidCommandTests, ReturnTrueIfNameHasUpperCases) {
    ASSERT_TRUE(internal::validCommand({"QUIT"}, {}, &Quit));
}

TEST(ValidCommandTests, ReturnTrueIfArgumentHasUpperCases) {
    ASSERT_TRUE(internal::validCommand({}, {"ARGS"}, &Quit));
}

TEST(ValidCommandTests, ReturnTrueIfNameHasLowerCases) {
    ASSERT_TRUE(internal::validCommand({"quit"}, {}, &Quit));
}

TEST(ValidCommandTests, ReturnFalseIfArgumentHasLowerCases) {
    ASSERT_FALSE(internal::validCommand({}, {"args"}, &Quit));
}

TEST(ValidCommandTests, ReturnTrueIfNameStartsWithCommandPrefix) {
    ASSERT_TRUE(internal::validCommand({"@quit"}, {}, &Quit));
}

TEST(ValidCommandTests, ReturnFalseIfArgumentStartsWithCommandPrefix) {
    ASSERT_FALSE(internal::validCommand({}, {"@ARGS"}, &Quit));
}

TEST(ValidCommandTests, ForNameCommandPrefixIsNotAllowedOtherThanStart) {
    ASSERT_FALSE(internal::validCommand({"quit@"}, {}, &Quit));
}

TEST(ValidCommandTests, ForArgumentCommandPrefixIsNotAllowedOtherThanStart) {
    ASSERT_FALSE(internal::validCommand({}, {"ARGS@"}, &Quit));
}

TEST(ValidCommandTests, ReturnTrueIfGivenVariadicArgument) {
    ASSERT_TRUE(internal::validCommand({}, {VARIADIC_ARGUMENT}, &Quit));
}

TEST(ValidCommandTests, OptionalArgumentIsAllowed) {
    ASSERT_TRUE(internal::validCommand({}, {"[ARGS]"}, &Quit));
}
