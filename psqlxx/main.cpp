#include <pqxx/pqxx>

#include <psqlxx/args.hpp>
#include <psqlxx/cli.hpp>
#include <psqlxx/db.hpp>


using namespace psqlxx;


namespace {

[[nodiscard]]
inline const auto buildOptions() {
    auto options = CreateBaseOptions();

    AddDbOptions(options);

    return options;
}

[[nodiscard]]
inline const auto
handleOptions(cxxopts::Options &options, const int argc, char **argv) {
    const auto results = ParseOptions(options, argc, argv);
    if (not results) {
        exit(EXIT_FAILURE);
    }

    HandleBaseOptions(options, results.value());

    return HandleDbOptions(results.value());
}

[[nodiscard]]
inline constexpr auto toExitCode(const bool success) {
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

[[nodiscard]]
inline const std::unique_ptr<FILE, decltype(&fclose)>
OpenCommandFile(const std::string &command_file) {
    FILE *file_ptr{};
    if (not command_file.empty()) {
        file_ptr = fopen(command_file.c_str(), "r");
        if (not file_ptr) {
            perror(("Failed to open command file '" + command_file + "'").c_str());
            exit(EXIT_FAILURE);
        }
    }

    return {file_ptr, &fclose};
}

}


int main(int argc, char **argv) {
    auto options = buildOptions();

    const auto connection_options = handleOptions(options, argc, argv);

    const auto my_connection = MakeConnection(connection_options);
    if (not my_connection) {
        return EXIT_FAILURE;
    }

    if (connection_options.list_DBs_and_exit) {
        return toExitCode(ListDbs(my_connection));
    }

    if (not connection_options.commands.empty()) {
        for (const auto &a_command : connection_options.commands) {
            if (not DoTransaction(my_connection, a_command)) {
                return EXIT_FAILURE;
            }
        }

        return EXIT_SUCCESS;
    }

    const auto command_file_ptr = OpenCommandFile(connection_options.command_file);

    Cli my_cli{argv[0], CliOptions{command_file_ptr.get()}};
    my_cli.Config();
    my_cli.RegisterCommandGroup(CreatePsqlxxCommandGroup(my_connection));

    return toExitCode(my_cli.Run());
}
