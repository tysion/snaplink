#include "url.hpp"

#include <userver/formats/json/value_builder.hpp>

namespace snaplink::dto {

userver::formats::json::Value
Serialize(const Url& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["id"] = data.id;
    builder["link"] = data.link;
    return builder.ExtractValue();
}

}  // namespace snaplink::dto