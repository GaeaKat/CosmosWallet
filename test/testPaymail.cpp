// Copyright (c) 2021 Katrina Knight
// Distributed under the Open BSV software license, see the accompanying file LICENSE.


#include <utils/paymail/paymail.hpp>
#include <gmock/gmock-matchers.h>
#include "gtest/gtest.h"

namespace Gigamonkey::Bitcoin::Paymail {

    class PayMailTest: public Paymail, public testing::Test
    {

    };
    TEST_F(PayMailTest,ValidateAddress) {
        EXPECT_TRUE( isValid("nekotest3000@simply.cash")) ;
        EXPECT_FALSE(isValid("nekotest3000simply.cash"));
        EXPECT_FALSE(isValid("nekotest3000@simply"));
        EXPECT_FALSE(isValid("simply.cash"));
    }
    TEST_F(PayMailTest,ValidateSplit) {
        auto cur=splitAddress("nekotest3000@simply.cash");
        EXPECT_EQ(cur.first,"nekotest3000");
        EXPECT_EQ(cur.second,"simply.cash");

        auto nxt=splitAddress("nekotest3000simply.cash");
        EXPECT_EQ(nxt.first,"");
        EXPECT_EQ(nxt.second,"");

    }

    TEST_F(PayMailTest, GetCapabilities) {
        auto tmp= getCapabilities("simply.cash");
        std::cout << tmp.size();
    }

    TEST_F(PayMailTest, GetScript) {
        auto tmp= getScript("nekotest3000@simply.cash","idontexist@simply.cash",11,"Test paymail functionality","Katrina Knight");

        ASSERT_THAT(tmp, ::testing::ElementsAre(0x76, 0xa9, 0x14, 0x2b, 0x05, 0x4c, 0xed, 0x88,0xc9,0x2a,0x30,0xcc,0x63,0x8f,0x93,0x5a,0xc0,0x6b,0x21,0x3a,0x2c,0x75,0x24,0x88,0xac));
        auto tmp2= getScript("katgaea@centbee.com","Idontcare@simply.cash");
        std::cout << tmp2 << std::endl;
        std::cout << tmp << std::endl;
    }
}