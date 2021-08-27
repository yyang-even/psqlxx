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

auto &printStrInCenter(std::ostream &out, const std::string_view a_field,
                       const int width) {
    const int padding = width - a_field.size();
    const auto half_spaces = std::string(padding > 1 ? padding / 2 : 0, ' ');
    out << half_spaces << a_field << half_spaces;
    if (padding > 0 and padding % 2 != 0) {
        out << ' ';
    }

    return out;
}

auto &printField(std::ostream &out, const std::string_view a_field,
                 const std::string_view special_chars, const int width) {
    const auto do_quote = (not special_chars.empty()) and
                          (a_field.find_first_of(special_chars) != std::string_view::npos);
    if (do_quote) {
        out << '"';
    }
    printStrInCenter(out, a_field, width);
    if (do_quote) {
        out << '"';
    }

    return out;
}

inline auto &printFieldBar(std::ostream &out, const int width) {
    std::string bar(width, '-');
    return out << bar;
}

[[nodiscard]]
auto getColumnWidths(const pqxx::result &a_result, const bool no_align) {
    std::vector<std::size_t> widths(a_result.columns(), 0);

    if (no_align) {
        return widths;
    }

    for (std::size_t i = 0; i < widths.size(); ++i) {
        widths[i] = std::strlen(a_result.column_name(i));
    }

    for (const auto &row : a_result) {
        if (not row.empty()) {
            for (std::size_t i = 0; i < widths.size(); ++i) {
                widths[i] = std::max(widths[i], row[i].view().size());
            }
        }
    }

    for (auto &w : widths) {
        w += 2;
    }

    return widths;
}

}


namespace psqlxx {

void AddFormatOptions(cxxopts::Options &options) {
    options.add_options("Formatter")
    ("A,no-align", "unaligned table output mode",
     cxxopts::value<bool>()->default_value("false"))
    ("csv", "CSV (Comma-Separated Values) table output mode",
     cxxopts::value<bool>()->default_value("false"))

    ("F,field-separator", "field separator for unaligned output",
     cxxopts::value<std::string>()->default_value("|"))

    ("o,out-file", "send query results to file",
     cxxopts::value<std::string>()->default_value(""))
    ;
}

FormatterOptions HandleFormatOptions(const cxxopts::ParseResult &parsed_options) {
    FormatterOptions options{};

    options.out_file = parsed_options["out-file"].as<std::string>();

    if (parsed_options["csv"].as<bool>()) {
        options.delimiter = ",";
        options.special_chars = options.delimiter + "\n\r";
        options.show_title_and_summary = false;
        options.no_align = true;
    } else {
        options.delimiter = parsed_options["field-separator"].as<std::string>();
        options.no_align = parsed_options["no-align"].as<bool>();
    }

    return options;
}

void PrintResult(const pqxx::result &a_result, const FormatterOptions &options,
                 std::ostream &out, const std::string_view title) {
    if (a_result.columns() > 0) {
        if (options.show_title_and_summary and (not title.empty())) {
            out << title << std::endl;
        }

        const auto column_widths = getColumnWidths(a_result, options.no_align);

        for (std::size_t i = 0; i < column_widths.size() - 1; ++i) {
            printField(out, a_result.column_name(i), options.special_chars,
                       column_widths[i]) << options.delimiter;
        }
        printField(out, a_result.column_name(a_result.columns() - 1), options.special_chars,
                   column_widths.back()) << std::endl;

        if (not options.no_align) {
            for (std::size_t i = 0; i < column_widths.size() - 1; ++i) {
                printFieldBar(out, column_widths[i]) << '+';
            }
            printFieldBar(out, column_widths.back()) << std::endl;
        }

        for (const auto &row : a_result) {
            if (not row.empty()) {
                for (std::size_t i = 0; i < column_widths.size() - 1; ++i) {
                    printField(out, row[i].view(), options.special_chars,
                               column_widths[i]) << options.delimiter;
                }
                printField(out, row.back().view(), options.special_chars,
                           column_widths.back()) << std::endl;
            }
        }

        if (options.show_title_and_summary) {
            printSummary(out, a_result.size()) << std::endl;
        }
    }
}

}//namespace psqlxx
