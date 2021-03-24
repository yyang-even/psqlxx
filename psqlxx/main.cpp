#include <pqxx/pqxx>

#include <psqlxx/args.hpp>
#include <psqlxx/cli.hpp>
#include <psqlxx/db.hpp>


using namespace psqlxx;


namespace {

[[nodiscard]]
const auto buildOptions() {
    auto options = CreateBaseOptions();

    AddDbOptions(options);

    return options;
}

[[nodiscard]]
const auto handleOptions(cxxopts::Options &options, int argc, char **argv) {
    const auto results = ParseOptions(options, argc, argv);
    if (not results) {
        exit(EXIT_FAILURE);
    }

    HandleBaseOptions(options, results.value());

    return HandleDbOptions(results.value());
}

}


int main(int argc, char **argv) {
    auto options = buildOptions();

    const auto connection_options = handleOptions(options, argc, argv);

    const auto my_connection = MakeConnection(connection_options);
    if (not my_connection) {
        exit(EXIT_FAILURE);
    }

    Cli my_cli{argv[0], CliOptions{}};
    my_cli.Config();
    my_cli.RegisterLineHandler([my_connection](const char *sql_cmd) {
        DoTransaction(my_connection, sql_cmd);
    });
    my_cli.Run();

    return EXIT_SUCCESS;
}
