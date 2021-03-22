#pragma once

#include <cxxopts.hpp>


namespace psqlxx {

const cxxopts::Options CreateBaseOptions();

const std::optional<cxxopts::ParseResult>
ParseOptions(cxxopts::Options &options, int argc, char **argv) noexcept;

void HandleBaseOptions(const cxxopts::Options &options,
                       const cxxopts::ParseResult &parsed_options);

}//namespace psqlxx
