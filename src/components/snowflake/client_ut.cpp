#include "client.hpp"

#include <userver/utest/utest.hpp>

using namespace snaplink::components::snowflake;

UTEST(SnowflakeTest, GeneratesUniqueIds) {
    Client generator(NodeId{1});

    std::set<int64_t> ids;
    for (int i = 0; i < 1000; ++i) {
        auto id = generator.GenerateId().GetUnderlying();
        EXPECT_TRUE(ids.insert(id).second) << "Duplicate ID found: " << id;
    }
}

UTEST(SnowflakeTest, IdsAreOrdered) {
    Client generator(NodeId{1});

    int64_t prev_id = generator.GenerateId().GetUnderlying();
    for (int i = 0; i < 1000; ++i) {
        int64_t new_id = generator.GenerateId().GetUnderlying();
        EXPECT_GT(new_id, prev_id) << "IDs are not strictly increasing";
        prev_id = new_id;
    }
}

UTEST(SnowflakeTest, SequenceRollsOver) {
    Client generator(NodeId{1});

    int64_t first_id = generator.GenerateId().GetUnderlying();
    int sequence = 1;

    while (sequence < (1 << Client::kSequenceBits)) {
        generator.GenerateId();
        ++sequence;
    }

    int64_t next_id = generator.GenerateId().GetUnderlying();
    EXPECT_GT(next_id, first_id) << "Sequence did not roll over correctly";
}

UTEST(SnowflakeTest, IdDecoding) {
    Client generator(NodeId{5});
    auto id = generator.GenerateId().GetUnderlying();

    int64_t timestamp = (id >> (Client::kNodeBits + Client::kSequenceBits)) + Client::kEpoch;
    int64_t node_id = (id >> Client::kSequenceBits) & ((1 << Client::kNodeBits) - 1);
    int64_t sequence = id & ((1 << Client::kSequenceBits) - 1);

    EXPECT_EQ(node_id, 5);
    EXPECT_GE(timestamp, Client::kEpoch);
    EXPECT_GE(sequence, 0);
}