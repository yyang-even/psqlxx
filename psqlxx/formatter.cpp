#include <psqlxx/formatter.hpp>

#include <cxxopts.hpp>
#include <pqxx/pqxx>


namespace {

inline auto &printSummary(std::ostream &out, const std::size_t size) {
    out << "(" << size << " row";
    if (size > 1) {
        out << 's';
    }
    return out << ")";
}

inline auto &printField(std::ostream &out, const pqxx::field &a_field,
                        const std::string_view special_chars) {
    const auto do_quote = (not special_chars.empty()) and
                          (a_field.view().find_first_of(special_chars) != std::string_view::npos);
    if (do_quote) {
        out << '"';
    }
    out << a_field;
    if (do_quote) {
        out << '"';
    }

    return out;
}

}


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
        options.special_chars = options.delimiter + "\n\r";
        options.show_title_and_summary = false;
    } else {
        options.delimiter = parsed_options["field-separator"].as<std::string>();
    }

    return options;
}

void PrintResult(const pqxx::result &a_result, const FormatterOptions &options,
                 std::ostream &out, const std::string_view title) {
    if (a_result.columns() > 0) {
        if (options.show_title_and_summary and (not title.empty())) {
            out << title << std::endl;
        }

        for (decltype(a_result.columns()) i = 0; i < a_result.columns() - 1; ++i) {
            out << a_result.column_name(i) << options.delimiter;
        }
        out << a_result.column_name(a_result.columns() - 1) << std::endl;

        for (const auto &row : a_result) {
            if (not row.empty()) {
                for (auto iter = row.cbegin(); iter != std::prev(row.cend()); ++iter) {
                    printField(out, *iter, options.special_chars) << options.delimiter;
                }
                printField(out, row.back(), options.special_chars) << std::endl;
            }
        }

        if (options.show_title_and_summary) {
            printSummary(out, a_result.size()) << std::endl;
        }
    }
}

}//namespace psqlxx
