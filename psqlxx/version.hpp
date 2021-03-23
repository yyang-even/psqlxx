#pragma once

#include <string>


namespace psqlxx {

[[nodiscard]]
const std::string GetVersion();

[[nodiscard]]
const std::string GetGitDescribe();

}//namespace psqlxx
