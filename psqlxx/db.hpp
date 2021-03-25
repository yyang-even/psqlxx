#pragma once

#include <memory>


namespace cxxopts {

class Options;
class ParseResult;

}

namespace pqxx {

class connection;

}


namespace psqlxx {

namespace internal {

/**
 * @note    password escaping and quoting are currently not supported.
 */
[[nodiscard]]
const std::string overridePassword(std::string connection_string, const char *password);

}//namespace internal

struct DbOptions {
    std::string base_connection_string;

    bool prompt_for_password = true;
};


[[nodiscard]]
const std::shared_ptr<pqxx::connection> MakeConnection(const DbOptions &options);

void DoTransaction(const std::shared_ptr<pqxx::connection> connection_ptr,
                   const char *sql_cmd);

void AddDbOptions(cxxopts::Options &options);

[[nodiscard]]
const DbOptions HandleDbOptions(const cxxopts::ParseResult &parsed_options);


enum class DbParameterKey {
    host,
    port,
    dbname,
    user,
    password,
};

[[nodiscard]]
const std::string
ComposeDbParameter(const DbParameterKey key_enum, const std::string &value);

}//namespace psqlxx
