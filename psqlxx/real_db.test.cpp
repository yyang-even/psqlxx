#include <psqlxx/db.hpp>

#include <pqxx/pqxx>
#include <gtest/gtest.h>

#include <psqlxx/test_utils.hpp>


using namespace psqlxx;
using namespace test;


TEST(MakeConnectionTests, ReturnExpectedIfGivenValidParametersWithoutPrompt) {
    ConnectionOptions options;
    options.prompt_for_password = false;
    options.base_connection_string = GetViewerConnectionString();

    ASSERT_TRUE(internal::makeConnection(options));
}

TEST(MakeConnectionTests, ReturnExpectedIfGivenValidParametersWithPrompt) {
    ConnectionOptions options;
    options.prompt_for_password = true;
    options.base_connection_string = GetViewerConnectionString();

    ASSERT_TRUE(internal::makeConnection(options));
}

TEST(MakeConnectionTests, ReturnNullptrIfGivenWrongPasswordAndNoPrompt) {
    ConnectionOptions options;
    options.prompt_for_password = false;
    options.base_connection_string =
        internal::overridePassword(GetViewerConnectionString(), "wrong_password");

    ASSERT_FALSE(internal::makeConnection(options));
}

