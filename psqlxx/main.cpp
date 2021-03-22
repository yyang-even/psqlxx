#include <pqxx/pqxx>

#include <psqlxx/args.hpp>
#include <psqlxx/cli.hpp>
#include <psqlxx/sql_session.hpp>


using namespace psqlxx;


namespace {

const cxxopts::Options BuildOptions() {
    auto options = CreateBaseOptions();

    AddPqOptions(options);

    return options;
}

const std::string HandleOptions(cxxopts::Options &options, int argc, char **argv) {
    const auto results = ParseOptions(options, argc, argv);

    if (not results) {
        exit(EXIT_FAILURE);
    }

    HandleBaseOptions(options, results.value());

    return HandlePqOptions(results.value());
}

}


int main(int argc, char **argv) {
    auto options = BuildOptions();

    const auto connection_str = HandleOptions(options, argc, argv);

    const auto my_connection = std::make_shared<pqxx::connection>(connection_str);  //throws
    Cli my_cli{argv[0], CliOptions{}};
    my_cli.Config();
    my_cli.RegisterLineHandler([my_connection](const char *sql_cmd) {
        DoTransaction(my_connection, sql_cmd);
    });
    my_cli.Run();

    return EXIT_SUCCESS;
}
