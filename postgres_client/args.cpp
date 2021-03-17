#include <postgres_client/args.hpp>
#include <postgres_client/version.hpp>


namespace postgres_client {

const cxxopts::Options CreateBaseOptions() {
    cxxopts::Options options("postgres_client", "Yet another Postgres client.");

    options.add_options()
    ("h,help", "Print usage")
    ("v,version", "Print version")
    ;

    return options;
}

const std::optional<cxxopts::ParseResult>
ParseOptions(cxxopts::Options &options, int argc, char **argv) noexcept {
    try {
        return options.parse(argc, argv);
    } catch (const cxxopts::option_not_exists_exception &e) {
        std::cout << "Unrecognised command-line options: " << e.what() << std::endl;
    } catch (const cxxopts::OptionException &e) {
        std::cerr << "Error parsing command-line options: " << e.what() << std::endl;
    }

    return {};
}

void HandleBaseOptions(const cxxopts::Options &options,
                       const cxxopts::ParseResult &parsed_options) {
    if (parsed_options.count("help")) {
        std::cout << options.help() << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (parsed_options.count("version")) {
        std::cout << "Version: " << GetVersion() << std::endl;
        std::cout << "Git Description: " << GetGitDescribe() << std::endl;
        exit(EXIT_SUCCESS);
    }
}

}//namespace postgres_client
