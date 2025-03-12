#include "components/snowflake/component.hpp"
#include "handlers/api/url/handler.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/auth/auth_checker_factory.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/server_monitor.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

int main(int argc, char* argv[]) {
    const auto component_list = userver::components::MinimalServerComponentList()
                                    .Append<userver::server::handlers::Ping>()
                                    .Append<userver::server::handlers::TestsControl>()
                                    .Append<userver::server::handlers::ServerMonitor>()
                                    .Append<userver::components::TestsuiteSupport>()
                                    .Append<userver::components::HttpClient>()
                                    .Append<userver::components::Postgres>("postgres-snaplink-database")
                                    .Append<userver::clients::dns::Component>()
                                    .Append<snaplink::components::snowflake::Component>()
                                    .Append<snaplink::handlers::api::url::get::Handler>()
                                    .Append<snaplink::handlers::api::url::post::Handler>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}