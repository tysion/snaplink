#include "client.hpp"

#include <userver/engine/sleep.hpp>
#include <userver/utils/datetime.hpp>

#include <chrono>

using namespace std::chrono_literals;

namespace snaplink::components::snowflake {

Id Client::GenerateId() {
    std::lock_guard<userver::engine::Mutex> lock(mutex_);

    int64_t timestamp = GetCurrentTimestamp();

    if (last_timestamp_ == timestamp) {
        sequence_ = (sequence_ + 1) & kSequenceMask;
        if (sequence_ == 0) {
            userver::engine::SleepFor(1ms);
            timestamp = GetCurrentTimestamp();
        }
    } else {
        sequence_ = 0;
    }

    last_timestamp_ = timestamp;

    int64_t id = 0;
    id |= (timestamp - kEpoch) << (kNodeBits + kSequenceBits);
    id |= (node_id_.GetUnderlying()) << kSequenceBits;
    id |= sequence_;
    return Id{id};
}

int64_t Client::GetCurrentTimestamp() {
    auto time_point = userver::utils::datetime::Now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch()).count();
}

Client::Client(NodeId node_id) : node_id_{node_id} {}

}  // namespace snaplink::components::snowflake