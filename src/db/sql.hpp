#pragma once

#include <string_view>

namespace snaplink::db::sql {

inline constexpr std::string_view kAddNewUrl = R"~(
SELECT snaplink.add_new_url($1, $2)
)~";

inline constexpr std::string_view kGetUrlById = R"~(
SELECT snaplink.get_url_by_id($1)
)~";

}  // namespace snaplink::db::sql