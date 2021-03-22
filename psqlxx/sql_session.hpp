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

struct PqOptions {
    std::string connection_string;

    bool prompt_for_password_if_not_supplied = true;

    const std::string GetConnectionString() const;
};


std::shared_ptr<pqxx::connection> MakeConnection(const PqOptions &options);

void DoTransaction(const std::shared_ptr<pqxx::connection> connection_ptr,
                   const char *sql_cmd);

void AddPqOptions(cxxopts::Options &options);

const PqOptions HandlePqOptions(const cxxopts::ParseResult &parsed_options);

}//namespace psqlxx
