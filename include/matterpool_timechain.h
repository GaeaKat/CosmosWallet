// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <gigamonkey/timechain.hpp>
#include "MatterpoolAPI.h"

#ifndef COSMOSWALLET_MATTERPOOL_TIMECHAIN_H
#define COSMOSWALLET_MATTERPOOL_TIMECHAIN_H

namespace Cosmos {
    class MatterPool_TimeChain: public Gigamonkey::Bitcoin::timechain {
    public:
        MatterPool_TimeChain(): rateLimit(100,60) {}


        data::list<Gigamonkey::uint<80>> headers(data::uint64 since_height) const override;

        data::bytes transaction(const Gigamonkey::digest<32> &digest) const override;
    private:
        void waitForRateLimit() const;
        Gigamonkey::Merkle::path merkle_path(const Gigamonkey::digest<32> &digest) const override;

        Gigamonkey::uint<80> header(const Gigamonkey::digest<32> &digest) const override;

        data::list<Gigamonkey::Bitcoin::txid> transactions(const Gigamonkey::digest<32> &digest) const override;

        data::bytes block(const Gigamonkey::digest<32> &digest) const override;
    private:
        mutable data::tools::rate_limiter rateLimit;
        mutable data::networking::Http http;

    };
}
#endif //COSMOSWALLET_MATTERPOOL_TIMECHAIN_H
