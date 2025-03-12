#pragma once

#include <userver/formats/json/value.hpp>
#include <userver/formats/serialize/to.hpp>

namespace snaplink::dto {

struct Url final {
    std::string id;
    std::string link;
};

userver::formats::json::Value
Serialize(const Url& data, userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace snaplink::dto