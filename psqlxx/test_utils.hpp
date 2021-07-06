#pragma once

#include <psqlxx/db.hpp>


namespace psqlxx {

namespace test {

[[nodiscard]]
std::string GetDbHostParameter();

[[nodiscard]]
std::string GetSharedDbNameParameter();

[[nodiscard]]
std::string GetViewerConnectionString();

[[nodiscard]]
std::string GetAdminConnectionString();

}//namespace test

}//namespace psqlxx
