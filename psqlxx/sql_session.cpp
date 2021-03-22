#include <psqlxx/sql_session.hpp>

#include <unistd.h>

#include <iostream>
#include <regex>

#include <cxxopts.hpp>
#include <pqxx/pqxx>


namespace {

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

auto connectionStringContainsPassword(const std::string &connection_string) {
    if (std::regex_search(connection_string, std::regex{"password ?="})) {
        return true;
    }

    if (std::regex_search(connection_string, std::regex{":[^']+@"})) {
        return true;
    }

    return false;
}

const auto appendPassword(std::string connection_string, const char *password) {
    assert(not connectionStringContainsPassword(connection_string));
    assert(password);

    if (connection_string.find("://") == std::string::npos) {
        connection_string.push_back(' ');
    } else if (connection_string.rfind('?') != std::string::npos) {
        connection_string.push_back('?');
    } else {
        connection_string.push_back('&');
    }

    return connection_string + "password=" + password;
}

}


namespace psqlxx {

const std::string PqOptions::GetConnectionString() const {
    if (prompt_for_password_if_not_supplied and
        not base_connection_string.empty() and
        not connectionStringContainsPassword(base_connection_string)) {
        const auto *password = getpass("Password: ");
        return appendPassword(base_connection_string, password);
    }

    return base_connection_string;
}

std::shared_ptr<pqxx::connection> MakeConnection(const PqOptions &options) {
    const auto connection_string = options.GetConnectionString();
    try {
        return std::make_shared<pqxx::connection>(connection_string);
    } catch (const pqxx::broken_connection &e) {
        std::cerr << e.what() << std::endl;
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

void AddPqOptions(cxxopts::Options &options) {
    options.add_options("PQXX")
    ("c,connection-string",
     "PQ connection string. Refer to the libpq connect call for a complete definition of what may go into the connect string. By default the client will try to connect to a server running on the local machine.",
     cxxopts::value<std::string>()->default_value(""))
    ("no-password", "never prompt for password",
     cxxopts::value<bool>()->default_value("false"))
    ;
}

const PqOptions HandlePqOptions(const cxxopts::ParseResult &parsed_options) {
    PqOptions options;
    options.base_connection_string = parsed_options["connection-string"].as<std::string>();
    options.prompt_for_password_if_not_supplied =
        not parsed_options["no-password"].as<bool>();

    return options;
}

}//namespace psqlxx
