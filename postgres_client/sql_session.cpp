#include <postgres_client/sql_session.hpp>

#include <iostream>

#include <cxxopts.hpp>
#include <pqxx/pqxx>


namespace {

const std::string GetTransactionName() {
    return "postgres_client";
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

            std::cout << "Result: " << r[0][0].c_str() << std::endl;

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
