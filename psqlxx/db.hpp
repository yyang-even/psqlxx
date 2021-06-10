#pragma once

#include <memory>
#include <string>
#include <vector>

#include <psqlxx/command.hpp>


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
const std::string overridePassword(std::string connection_string, std::string password);

}//namespace internal

struct DbOptions {
    std::vector<std::string> commands;

    std::string base_connection_string;
    std::string command_file;

    bool prompt_for_password = true;

    bool list_DBs_and_exit = false;

    [[nodiscard]]
    bool CommandsFromFile() const {
        return not command_file.empty();
    }

    [[nodiscard]]
    const auto OpenCommandFile() const {
        std::unique_ptr<FILE, decltype(&fclose)>
        command_file_ptr(fopen(command_file.c_str(), "r"), &fclose);
        if (not command_file_ptr) {
            perror(("Failed to open command file: '" + command_file + "'.").c_str());
        }
        return command_file_ptr;
    }

private:
};


[[nodiscard]]
const std::shared_ptr<pqxx::connection> MakeConnection(const DbOptions &options);

[[nodiscard]]
bool DoTransaction(const std::shared_ptr<pqxx::connection> connection_ptr,
                   const std::string_view sql_cmd);

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
ComposeDbParameter(const DbParameterKey key_enum, std::string value);

[[nodiscard]]
bool ListDbs(const std::shared_ptr<pqxx::connection> connection_ptr);

[[nodiscard]]
const CommandGroup
CreatePsqlxxCommandGroup(const std::shared_ptr<pqxx::connection> connection_ptr);

}//namespace psqlxx
