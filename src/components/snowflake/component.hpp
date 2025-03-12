#pragma once

#include "client.hpp"

#include <userver/components/loggable_component_base.hpp>

namespace snaplink::components::snowflake {

class Component final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "component-snowflake";

    Component(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

    Client& GetClient();

    static userver::yaml_config::Schema GetStaticConfigSchema();

private:
    Client client_;
};

}  // namespace snaplink::components::snowflake