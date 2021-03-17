#include <pqxx/pqxx>

#include <postgres_client/args.hpp>
#include <postgres_client/cli.hpp>
#include <postgres_client/sql_session.hpp>


using namespace postgres_client;


const cxxopts::Options BuildOptions() {
    auto options = CreateBaseOptions();

    return options;
}

const std::string HandleOptions(cxxopts::Options &options, int argc, char **argv) {
    const auto results = ParseOptions(options, argc, argv);

    HandleBaseOptions(options, results.value());    //throws

    return HandlePqOptions(results.value());   //throws
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
