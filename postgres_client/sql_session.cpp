#include <postgres_client/sql_session.hpp>

#include <iostream>

#include <cxxopts.hpp>
#include <pqxx/pqxx>


namespace {

const std::string GetTransactionName() {
    return "postgres_client";
}

void Print(const pqxx::result &a_result) {
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

}


namespace postgres_client {

void DoTransaction(const std::shared_ptr<pqxx::connection> connection_ptr,
                   const char *sql_cmd) {
    assert(connection_ptr);

    pqxx::perform([connection_ptr, sql_cmd] {
        try {
            pqxx::work w(*connection_ptr, GetTransactionName());

            auto r = w.exec(sql_cmd);

            Print(r);

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
    ;
}

const std::string HandlePqOptions(const cxxopts::ParseResult &parsed_options) {
    return parsed_options["connection-string"].as<std::string>();
}

}//namespace postgres_client
