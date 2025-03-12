#include "component.hpp"

#include <userver/components/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace snaplink::components::snowflake {

Component::Component(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::LoggableComponentBase(config, context), client_(NodeId{config["node_id"].As<int64_t>()}) {}

Client& Component::GetClient() { return client_; }

userver::yaml_config::Schema Component::GetStaticConfigSchema() {
    constexpr auto schema = R"(
        type: object
        description: Snowflake component for generating unique 64-bit IDs based on timestamp, node ID, and sequence number.
        additionalProperties: false
        properties:
            node_id:
                type: integer
                description: Unique identifier for the node (machine) in the Snowflake system. Used to differentiate instances generating IDs.

    )";

    return userver::yaml_config::MergeSchemas<userver::components::LoggableComponentBase>(schema);
}

}  // namespace snaplink::components::snowflake