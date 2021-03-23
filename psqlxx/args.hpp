#pragma once

#include <cxxopts.hpp>


namespace psqlxx {

[[nodiscard]]
const cxxopts::Options CreateBaseOptions();

[[nodiscard]]
const std::optional<cxxopts::ParseResult>
ParseOptions(cxxopts::Options &options, int argc, char **argv) noexcept;

void HandleBaseOptions(const cxxopts::Options &options,
                       const cxxopts::ParseResult &parsed_options);

}//namespace psqlxx
