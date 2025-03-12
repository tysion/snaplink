#include "base62.hpp"

#include <userver/utest/utest.hpp>

using namespace snaplink::common;

TEST(Base62EncodeTest, HandlesZero) { EXPECT_EQ(Base62Encode(0), "0"); }

TEST(Base62EncodeTest, HandlesSmallNumbers) {
    EXPECT_EQ(Base62Encode(1), "1");
    EXPECT_EQ(Base62Encode(10), "A");
    EXPECT_EQ(Base62Encode(35), "Z");
    EXPECT_EQ(Base62Encode(36), "a");
    EXPECT_EQ(Base62Encode(61), "z");
}

TEST(Base62EncodeTest, HandlesLargerNumbers) {
    EXPECT_EQ(Base62Encode(62), "10");
    EXPECT_EQ(Base62Encode(12345), "3D7");
    EXPECT_EQ(Base62Encode(999999), "4C91");
    EXPECT_EQ(Base62Encode(987654321), "14q60P");
}

TEST(Base62EncodeTest, HandlesNegativeNumbers) { EXPECT_THROW(Base62Encode(-1), std::invalid_argument); }