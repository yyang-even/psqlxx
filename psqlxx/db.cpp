#include <psqlxx/db.hpp>
#include <psqlxx/string_utils.hpp>

#include <unistd.h>

#include <iostream>
#include <unordered_map>

#include <cxxopts.hpp>
#include <pqxx/pqxx>


namespace {

[[nodiscard]]
const std::string getTransactionName() {
    return "psqlxx";
}

void printResult(const pqxx::result &a_result) {
    if (a_result.columns() > 0) {
        for (decltype(a_result.columns()) i = 0; i < a_result.columns() - 1; ++i) {
            std::cout << a_result.column_name(i) << " | ";
        }
        std::cout << a_result.column_name(a_result.columns() - 1) << std::endl;

        for (const auto &row : a_result) {
            if (not row.empty()) {
                for (auto iter = row.cbegin(); iter != std::prev(row.cend()); ++iter) {
                    std::cout << *iter << " | ";
                }
                std::cout << row.back() << std::endl;
            }
        }
    }
}

[[nodiscard]]
const auto overridePasswordFromPrompt(const std::string &connection_string) {
    const auto *password = getpass("Password: ");
    return psqlxx::internal::overridePassword(connection_string, password);
}

[[nodiscard]]
const auto concatenateKeyValue(const std::string &key, const std::string &value) {
    return key + "=" + value;
}

}


namespace psqlxx {

namespace internal {

const std::string overridePassword(std::string connection_string,
                                   const char *password) {
    assert(password);

    const auto was_connection_str_empty = connection_string.empty();

    if (StartsWith(connection_string, "postgresql://") or
        StartsWith(connection_string, "postgres://")) {
        if (connection_string.rfind('?') == std::string::npos) {
            connection_string.push_back('?');
        } else {
            connection_string.push_back('&');
        }
    } else {
        if (not was_connection_str_empty) {
            connection_string.push_back(' ');
        }
    }

    return connection_string + ComposeDbParameter(DbParameterKey::password, password);
}

}//namespace internal

const std::shared_ptr<pqxx::connection> MakeConnection(const DbOptions &options) {
    for (bool original_tried = false; true; original_tried = true) {
        try {
            if (original_tried and options.prompt_for_password) {
                const auto connection_string =
                    overridePasswordFromPrompt(options.base_connection_string);
                return std::make_shared<pqxx::connection>(connection_string);
            } else {
                return std::make_shared<pqxx::connection>(options.base_connection_string);
            }
        } catch (const pqxx::broken_connection &e) {
            if (original_tried or not strstr(e.what(), "no password supplied")) {
                std::cerr << e.what() << std::endl;
                break;
            }
        }
    }

    return {};
}

void DoTransaction(const std::shared_ptr<pqxx::connection> connection_ptr,
                   const char *sql_cmd) {
    assert(connection_ptr);

    pqxx::perform([connection_ptr, sql_cmd] {
        try {
            pqxx::work w(*connection_ptr, getTransactionName());

            auto r = w.exec(sql_cmd);

            printResult(r);

        } catch (const std::exception &e) {
            std::cerr << e.what()
                      << std::endl;
        }
    });
}

void AddDbOptions(cxxopts::Options &options) {
    options.add_options("PQXX")
    ("c,connection-string",
     "PQ connection string. Refer to the libpq connect call for a complete definition of what may go into the connect string. By default the client will try to connect to a server running on the local machine.",
     cxxopts::value<std::string>()->default_value(""))
    ("no-password", "never prompt for password",
     cxxopts::value<bool>()->default_value("false"))
    ;
}

const DbOptions HandleDbOptions(const cxxopts::ParseResult &parsed_options) {
    DbOptions options;
    options.base_connection_string = parsed_options["connection-string"].as<std::string>();
    options.prompt_for_password = not parsed_options["no-password"].as<bool>();

    return options;
}

const std::string
ComposeDbParameter(const DbParameterKey key_enum, const std::string &value) {
    const static std::unordered_map<DbParameterKey, std::string> DB_PARAMETER_KEY_MAP = {
        {DbParameterKey::host, "host"},
        {DbParameterKey::port, "port"},
        {DbParameterKey::dbname, "dbname"},
        {DbParameterKey::user, "user"},
        {DbParameterKey::password, "password"},
    };

    return concatenateKeyValue(DB_PARAMETER_KEY_MAP.at(key_enum), value);
}

}//namespace psqlxx
