#include <filesystem>

#include <pqxx/pqxx>

#include <psqlxx/args.hpp>
#include <psqlxx/cli.hpp>
#include <psqlxx/db.hpp>


namespace fs = std::filesystem;
using namespace psqlxx;


namespace {

[[nodiscard]]
inline auto buildOptions() {
    auto options = CreateBaseOptions();

    AddDbProxyOptions(options);

    return options;
}

[[nodiscard]]
inline auto
handleOptions(cxxopts::Options &options, const int argc, char **argv) {
    const auto results = ParseOptions(options, argc, argv);
    if (not results) {
        exit(EXIT_FAILURE);
    }

    HandleBaseOptions(options, results.value());

    return HandleDbProxyOptions(results.value());
}

[[nodiscard]]
inline constexpr auto toExitCode(const bool success) {
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

[[nodiscard]]
inline std::unique_ptr<FILE, decltype(&fclose)>
OpenCommandFile(const std::string &command_file) {
    FILE *file_ptr{};
    if (not command_file.empty()) {
        file_ptr = fopen(command_file.c_str(), "r");
        if (not file_ptr) {
            std::cerr << "Failed to open command file '" <<
                      command_file << "': " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    return {file_ptr, &fclose};
}

}


int main(int argc, char **argv) {
    auto options = buildOptions();

    DbProxy db_proxy{handleOptions(options, argc, argv)};
    if (not db_proxy) {
        return EXIT_FAILURE;
    }

    const auto &proxy_options = db_proxy.GetOptions();

    if (proxy_options.list_DBs_and_exit) {
        return toExitCode(ListDbs(db_proxy));
    }

    if (not proxy_options.commands.empty()) {
        for (const auto &a_command : proxy_options.commands) {
            if (not db_proxy.DoTransaction(a_command)) {
                return EXIT_FAILURE;
            }
        }

        return EXIT_SUCCESS;
    }

    const auto command_file_ptr = OpenCommandFile(proxy_options.command_file);

    Cli my_cli{CliOptions{fs::path(argv[0]).stem(), command_file_ptr.get()}};
    my_cli.Config();
    my_cli.RegisterCommandGroup(CreatePsqlxxCommandGroup(db_proxy));

    return toExitCode(my_cli.Run());
}
