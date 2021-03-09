#include <postgres_client/args.hpp>

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

    return EXIT_SUCCESS;
}
