#include <postgres_client/args.hpp>
#include <postgres_client/version.hpp>


namespace postgres_client {

const cxxopts::Options BuildOptions() {
    cxxopts::Options options("postgres_client", "Yet another Postgres client.");

    options.add_options()
    ("h,help", "Print usage")
    ("v,version", "Print version")
    ;

    return options;
}


int HandleOptions(cxxopts::Options &options, int argc, char **argv) {
    try {
        const auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (result.count("version")) {
            std::cout << "Version: " << GetVersion() << std::endl;
            std::cout << "Git Description: " << GetGitDescribe() << std::endl;
            exit(EXIT_SUCCESS);
        }

    } catch (const cxxopts::option_not_exists_exception &e) {
        std::cout << "Unrecognised command-line options: " << e.what() << std::endl;
    } catch (const cxxopts::OptionException &e) {
        std::cerr << "Error parsing command-line options: " << e.what() << std::endl;
    }

    return 0;
}

}//namespace postgres_client
