#include <postgres_client/args.hpp>


using namespace postgres_client;


int main(int argc, char **argv) {
    auto options = BuildOptions();

    HandleOptions(options, argc, argv);

    return EXIT_SUCCESS;
}
