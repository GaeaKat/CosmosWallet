// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "matterpool_timechain.h"
#include "MatterpoolAPI.h"
#include "gtest/gtest.h"

namespace Cosmos {
    TEST(HEADER,BasicTest) {
        Cosmos::MatterpoolApi test;
        auto ret=test.headers(671710);
        std::cout << ret.size();
    }
}