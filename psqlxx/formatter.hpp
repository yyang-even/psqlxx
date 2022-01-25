#pragma once

#include <iostream>


namespace cxxopts {

class Options;
class ParseResult;

}

namespace pqxx {

class result;

}


namespace psqlxx {

struct FormatterOptions {
    std::string out_file;

    std::string delimiter;
    std::string special_chars;

    bool show_title_and_summary = true;
};

void AddFormatOptions(cxxopts::Options &options);

[[nodiscard]]
FormatterOptions HandleFormatOptions(const cxxopts::ParseResult &parsed_options);


void PrintResult(const pqxx::result &a_result, const FormatterOptions &options,
                 std::ostream &out, const std::string_view title);

}//namespace psqlxx
