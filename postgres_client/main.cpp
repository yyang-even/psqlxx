#include <postgres_client/args.hpp>
#include <postgres_client/cli.hpp>

#include <pqxx/pqxx>


using namespace postgres_client;


void DoQuery() {
    try {
        pqxx::connection c;
        pqxx::work w(c);

        pqxx::row r = w.exec1("SELECT 1");

        w.commit();

        std::cout << "Result: " << r[0].as<int>() << std::endl;
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char **argv) {
    auto options = BuildOptions();

    HandleOptions(options, argc, argv);

    DoQuery();
    Cli my_cli{argv[0], CliOptions{}};
    my_cli.Config();
    my_cli.Run();

    return EXIT_SUCCESS;
}
