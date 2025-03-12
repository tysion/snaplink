#pragma once

#include <userver/engine/mutex.hpp>
#include <userver/utils/strong_typedef.hpp>

namespace snaplink::components::snowflake {

using NodeId = userver::utils::StrongTypedef<class NodeIdTag, int64_t>;
using Id = userver::utils::StrongTypedef<class IdTag, int64_t>;

class Client {
public:
    static constexpr int64_t kEpoch = 1700000000000L;
    static constexpr int64_t kNodeBits = 10;
    static constexpr int64_t kSequenceBits = 12;
    static constexpr int64_t kSequenceMask = (1 << kSequenceBits) - 1;

    Client(NodeId node_id);

    Id GenerateId();

    static int64_t GetCurrentTimestamp();

private:
    const NodeId node_id_;
    int64_t last_timestamp_;
    int64_t sequence_;
    userver::engine::Mutex mutex_;
};

}  // namespace snaplink::components::snowflake