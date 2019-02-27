#pragma once

namespace postgres {

inline constexpr auto CONNECT_STR = "user=cxx_client password=cxx_client dbname=cxx_client";
inline constexpr auto CONNECT_URI = "postgresql://cxx_client:cxx_client@/cxx_client";

}  // namespace postgres
