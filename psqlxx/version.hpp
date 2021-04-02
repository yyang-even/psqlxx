#pragma once

#include <string_view>


namespace psqlxx {

[[nodiscard]]
const std::string_view GetVersion();

[[nodiscard]]
const std::string_view GetGitDescribe();

}//namespace psqlxx
