#include <psqlxx/formatter.hpp>

#include <cxxopts.hpp>
#include <pqxx/pqxx>


namespace psqlxx {

void AddFormatOptions(cxxopts::Options &options) {
    options.add_options("Formatter")
    ("A,no-align", "unaligned table output mode",
     cxxopts::value<bool>()->default_value("true"))
    ("csv", "CSV (Comma-Separated Values) table output mode",
     cxxopts::value<bool>()->default_value("false"))

    ("F,field-separator", "field separator for unaligned output",
     cxxopts::value<std::string>()->default_value("|"))

    ("o,out-file", "send query results to file",
     cxxopts::value<std::string>()->default_value(""))
    ;
}

const FormatterOptions HandleFormatOptions(const cxxopts::ParseResult &parsed_options) {
    FormatterOptions options{};

    options.out_file = parsed_options["out-file"].as<std::string>();

    if (parsed_options["csv"].as<bool>()) {
        options.delimiter = ",";
    } else {
        options.delimiter = parsed_options["field-separator"].as<std::string>();
    }

    return options;
}

void PrintResult(const pqxx::result &a_result, const FormatterOptions &options,
                 std::ostream &out) {
    if (a_result.columns() > 0) {
        for (decltype(a_result.columns()) i = 0; i < a_result.columns() - 1; ++i) {
            out << a_result.column_name(i) << options.delimiter;
        }
        out << a_result.column_name(a_result.columns() - 1) << std::endl;

        for (const auto &row : a_result) {
            if (not row.empty()) {
                for (auto iter = row.cbegin(); iter != std::prev(row.cend()); ++iter) {
                    out << *iter << options.delimiter;
                }
                out << row.back() << std::endl;
            }
        }
    }
}

}//namespace psqlxx
