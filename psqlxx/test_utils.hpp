#pragma once

#include <psqlxx/db.hpp>


namespace psqlxx {

namespace test {

[[nodiscard]]
const std::string GetDbHostParameter();

[[nodiscard]]
const std::string GetSharedDbNameParameter();

[[nodiscard]]
const std::string GetViewerConnectionString(const bool with_password = true);

[[nodiscard]]
const std::string GetAdminConnectionString();

}//namespace test

}//namespace psqlxx
