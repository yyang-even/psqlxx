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

void DoTransaction(const std::shared_ptr<pqxx::connection> connection_ptr,
                   const char *sql_cmd);

void AddPqOptions(cxxopts::Options &options);

const std::string HandlePqOptions(const cxxopts::ParseResult &parsed_options);

}//namespace psqlxx
