#include "handler.hpp"

#include "common/base62.hpp"
#include "components/snowflake/client.hpp"
#include "components/snowflake/component.hpp"
#include "db/sql.hpp"
#include "dto/url.hpp"

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

inline constexpr std::string_view kHeaderLocation = "location";

namespace snaplink::handlers::api::url::post {

Handler::Handler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : HttpHandlerJsonBase(config, context),
      pg_cluster_{context.FindComponent<userver::components::Postgres>("postgres-snaplink-database").GetCluster()},
      snowflake_{context.FindComponent<components::snowflake::Component>().GetClient()} {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    [[maybe_unused]] const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& body,
    [[maybe_unused]] userver::server::request::RequestContext& context
) const {
    LOG_INFO() << "Received request to shorten a URL";

    if (!body.HasMember("link")) {
        LOG_ERROR() << "Request is missing 'link' field";
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Missing 'link' field in request body"}
        );
    }

    auto link = body["link"].As<std::string>();
    LOG_DEBUG() << "Extracted link: " << link;

    auto id = common::Base62Encode(snowflake_.GenerateId().GetUnderlying());
    LOG_DEBUG() << "Generated ID: " << id;

    try {
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster, db::sql::kAddNewUrl.data(), id, link
        );
        LOG_INFO() << "Successfully stored URL with ID: " << id;
    } catch (const std::exception& ex) {
        LOG_ERROR() << "Failed to store URL in database: " << ex.what();
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{"Database error"});
    }

    return userver::formats::json::MakeObject("id", id);
}

}  // namespace snaplink::handlers::api::url::post

namespace snaplink::handlers::api::url::get {

Handler::Handler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : HttpHandlerJsonBase(config, context),
      pg_cluster_{context.FindComponent<userver::components::Postgres>("postgres-snaplink-database").GetCluster()} {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    [[maybe_unused]] const userver::formats::json::Value& body,
    [[maybe_unused]] userver::server::request::RequestContext& context
) const {
    auto id = request.GetPathArg("id");
    LOG_INFO() << "Received request to retrieve URL for ID: " << id;

    try {
        auto result =
            pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave, db::sql::kGetUrlById.data(), id);

        if (result.IsEmpty()) {
            LOG_WARNING() << "No URL found for ID: " << id;
            throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"ID not found"});
        }

        auto link = result.AsSingleRow<std::string>();
        LOG_DEBUG() << "Resolved ID " << id << " to URL: " << link;

        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kTemporaryRedirect);
        request.GetHttpResponse().SetHeader(kHeaderLocation, link);
        LOG_INFO() << "Redirecting to: " << link;
    } catch (const std::exception& ex) {
        LOG_ERROR() << "Failed to retrieve URL from database: " << ex.what();
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{"Database error"});
    }

    return {};
}

}  // namespace snaplink::handlers::api::url::get