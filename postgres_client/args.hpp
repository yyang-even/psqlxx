#pragma once

#include <cxxopts.hpp>


namespace postgres_client {

const cxxopts::Options BuildOptions();

int HandleOptions(cxxopts::Options &options, int argc, char **argv);

}//namespace postgres_client
