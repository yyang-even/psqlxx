#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <psqlxx/command.hpp>
#include <psqlxx/formatter.hpp>


namespace cxxopts {

class Options;
class ParseResult;

}

namespace pqxx {

class connection;

}


namespace psqlxx {

struct ConnectionOptions {
    std::string base_connection_string;

    bool prompt_for_password = true;
};

struct DbProxyOptions {
    ConnectionOptions connection_options;

    FormatterOptions format_options;

    std::vector<std::string> commands;

    std::string command_file;

    bool list_DBs_and_exit = false;

    DbProxyOptions(ConnectionOptions conn_opts, FormatterOptions format_opts) :
        connection_options(std::move(conn_opts)),
        format_options(std::move(format_opts)) {
    }
};


namespace internal {

/**
 * @note    password escaping and quoting are currently not supported.
 */
[[nodiscard]]
const std::string overridePassword(std::string connection_string, std::string password);

[[nodiscard]]
std::unique_ptr<pqxx::connection> makeConnection(const ConnectionOptions &options);

}//namespace internal


class DbProxy {
    DbProxyOptions m_options;

    std::unique_ptr<pqxx::connection> m_connection;

    std::ofstream m_out_file;
    mutable std::ostream m_out;

public:
    explicit DbProxy(DbProxyOptions options);

    [[nodiscard]]
    operator bool() const {
        return m_connection and (m_options.format_options.out_file.empty() or m_out_file);
    }

    [[nodiscard]]
    const auto &GetOptions() const {
        return m_options;
    }

    [[nodiscard]]
    bool DoTransaction(const std::string_view sql_cmd,
                       const std::string_view title = {}) const;
};

void AddDbProxyOptions(cxxopts::Options &options);

[[nodiscard]]
const DbProxyOptions HandleDbProxyOptions(const cxxopts::ParseResult &parsed_options);

enum class DbParameterKey {
    host,
    port,
    dbname,
    user,
    password,
};

[[nodiscard]]
const std::string
ComposeDbParameter(const DbParameterKey key_enum, std::string value);

[[nodiscard]]
bool ListDbs(const DbProxy &db_proxy);

[[nodiscard]]
const CommandGroup
CreatePsqlxxCommandGroup(const DbProxy &proxy);

}//namespace psqlxx
