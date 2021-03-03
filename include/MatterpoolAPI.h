// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "types.h"
#include <data/tools/rate_limiter.h>
#include <data/types.hpp>
#include <data/networking/http.h>

#ifndef COSMOSWALLET_MATTERPOOLAPI_H
#define COSMOSWALLET_MATTERPOOLAPI_H

namespace Cosmos {

    class MatterpoolApi {
    public:
        MatterpoolApi() : rateLimit(100,60) {}
        std::vector<BitcoinHeader::BitcoinHeader> headers(data::uint64 since_height) ;
        void waitForRateLimit() ;

        //data::bytes transaction(const Gigamonkey::digest<32> &digest) const;

        //Gigamonkey::Merkle::path merkle_path(const Gigamonkey::digest<32> &digest) const;

        //Gigamonkey::uint<80> header(const Gigamonkey::digest<32> &digest) const;

        //data::list<Gigamonkey::Bitcoin::txid> transactions(const Gigamonkey::digest<32> &digest) const;

        //data::bytes block(const Gigamonkey::digest<32> &digest) const ;
    private:
        mutable data::tools::rate_limiter rateLimit;
        mutable data::networking::Http http;
    };
}
#endif //COSMOSWALLET_MATTERPOOLAPI_H
