#include <postgres_client/args.hpp>
#include <postgres_client/cli.hpp>
#include <postgres_client/sql_session.hpp>


using namespace postgres_client;


int main(int argc, char **argv) {
    auto options = BuildOptions();

    HandleOptions(options, argc, argv);

    const auto my_session = std::make_shared<SqlSession>();
    Cli my_cli{argv[0], CliOptions{}};
    my_cli.Config();
    my_cli.RegisterLineHandler([my_session](const char *sql_cmd) {
        my_session->RunSingle(sql_cmd);
    });
    my_cli.Run();

    return EXIT_SUCCESS;
}
