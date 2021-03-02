// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "matterpool_timechain.h"
#include "MatterpoolAPI.h"
#include "gtest/gtest.h"
namespace Cosmos {
    TEST(Transactions,Transactions) {
        MatterPool_TimeChain timeChain;
        Gigamonkey::digest<32> digest("0x9aaf2ae7015fffee2f31c2ce071bd4cce76e668b65d3db0f30665a814dfda84e");
        auto output = timeChain.transaction(digest);
        std::cout << output << std::endl;
    }

}