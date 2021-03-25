#include <psqlxx/db.hpp>

#include <gtest/gtest.h>


using namespace psqlxx;


TEST(MakeConnectionTests, ReturnNullptrIfGivenBadConnectionString) {
    DbOptions options;
    options.prompt_for_password = true;
    options.base_connection_string = ComposeDbParameter(DbParameterKey::dbname,
                                                        "no_such_db");

    ASSERT_FALSE(MakeConnection(options));
}


const char *SIMPLE_PASSWORD = "mysecret";
const auto SIMPLE_PASSWORD_PARAMETER = ComposeDbParameter(DbParameterKey::password,
                                                          SIMPLE_PASSWORD);


TEST(overridePasswordTests, MinimumUriFormShallReturnExpected) {
    const std::string CONNECTION_STRING = "postgres://";
    const std::string EXPECTED = CONNECTION_STRING + '?' + SIMPLE_PASSWORD_PARAMETER;
    const auto actual = internal::overridePassword(CONNECTION_STRING, SIMPLE_PASSWORD);

    ASSERT_EQ(EXPECTED, actual);
}

TEST(overridePasswordTests, UriFormWithNoNamedParameterShallReturnExpected) {
    const std::string CONNECTION_STRING = "postgresql://other@localhost/otherdb";
    const std::string EXPECTED = CONNECTION_STRING + '?' + SIMPLE_PASSWORD_PARAMETER;
    const auto actual = internal::overridePassword(CONNECTION_STRING, SIMPLE_PASSWORD);

    ASSERT_EQ(EXPECTED, actual);
}

TEST(overridePasswordTests, UriFormWithPasswordShallReturnExpected) {
    const std::string CONNECTION_STRING = "postgresql://user:secret@localhost";
    const std::string EXPECTED = CONNECTION_STRING + '?' + SIMPLE_PASSWORD_PARAMETER;
    const auto actual = internal::overridePassword(CONNECTION_STRING, SIMPLE_PASSWORD);

    ASSERT_EQ(EXPECTED, actual);
}

TEST(overridePasswordTests, UriFormWithNamedPasswordShallReturnExpected) {
    const std::string CONNECTION_STRING =
        "postgresql://other@localhost/otherdb?password=abcedfg";
    const std::string EXPECTED = CONNECTION_STRING + '&' + SIMPLE_PASSWORD_PARAMETER;
    const auto actual = internal::overridePassword(CONNECTION_STRING, SIMPLE_PASSWORD);

    ASSERT_EQ(EXPECTED, actual);
}

TEST(overridePasswordTests, UriFormWithNamedParametersShallReturnExpected) {
    const std::string CONNECTION_STRING =
        "postgresql://other@localhost/otherdb?connect_timeout=10&application_name=myapp";
    const std::string EXPECTED = CONNECTION_STRING + '&' + SIMPLE_PASSWORD_PARAMETER;
    const auto actual = internal::overridePassword(CONNECTION_STRING, SIMPLE_PASSWORD);

    ASSERT_EQ(EXPECTED, actual);
}

TEST(overridePasswordTests, EmptyConnectionStrShallReturnExpected) {
    const std::string CONNECTION_STRING = "";
    const std::string EXPECTED = CONNECTION_STRING + SIMPLE_PASSWORD_PARAMETER;
    const auto actual = internal::overridePassword(CONNECTION_STRING, SIMPLE_PASSWORD);

    ASSERT_EQ(EXPECTED, actual);
}

TEST(overridePasswordTests, KeyValueFormShallReturnExpected) {
    const auto CONNECTION_STRING =
        ComposeDbParameter(DbParameterKey::dbname, "no_such_db");
    const std::string EXPECTED = CONNECTION_STRING + ' ' + SIMPLE_PASSWORD_PARAMETER;
    const auto actual = internal::overridePassword(CONNECTION_STRING, SIMPLE_PASSWORD);

    ASSERT_EQ(EXPECTED, actual);
}

TEST(overridePasswordTests, KeyValueFormWithPasswordShallReturnExpected) {
    const std::string CONNECTION_STRING = "dbname=no_such_db password='my other sec'";
    const std::string EXPECTED = CONNECTION_STRING + ' ' + SIMPLE_PASSWORD_PARAMETER;
    const auto actual = internal::overridePassword(CONNECTION_STRING, SIMPLE_PASSWORD);

    ASSERT_EQ(EXPECTED, actual);
}
