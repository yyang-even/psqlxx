#include <psqlxx/db.hpp>

#include <gtest/gtest.h>

#include <psqlxx/test_utils.hpp>


using namespace psqlxx;
using namespace test;


TEST(MakeConnectionTests, ReturnExpectedIfGivenValidParametersWithoutPrompt) {
    DbOptions options;
    options.prompt_for_password = false;
    options.base_connection_string = GetViewerConnectionString();

    ASSERT_TRUE(MakeConnection(options));
}

TEST(MakeConnectionTests, ReturnExpectedIfGivenValidParametersWithPrompt) {
    DbOptions options;
    options.prompt_for_password = true;
    options.base_connection_string = GetViewerConnectionString();

    ASSERT_TRUE(MakeConnection(options));
}

TEST(MakeConnectionTests, ReturnNullptrIfGivenWrongPasswordAndNoPrompt) {
    DbOptions options;
    options.prompt_for_password = false;
    options.base_connection_string =
        internal::overridePassword(GetViewerConnectionString(), "wrong_password");

    ASSERT_FALSE(MakeConnection(options));
}

