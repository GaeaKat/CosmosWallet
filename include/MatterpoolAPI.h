// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "types.h"
#include <data/tools/rate_limiter.h>
#include <data/types.hpp>
#include <data/networking/http.h>
#include <gigamonkey/hash.hpp>
#include <gigamonkey/ledger.hpp>

#ifndef COSMOSWALLET_MATTERPOOLAPI_H
#define COSMOSWALLET_MATTERPOOLAPI_H

namespace Cosmos {

    class MatterpoolApi {
    public:
        MatterpoolApi() : rateLimit(100,60) {}
        std::vector<Gigamonkey::Bitcoin::header> headers(data::uint64 since_height) ;
        void waitForRateLimit() ;

        data::bytes transaction(const Gigamonkey::digest<32> &digest) ;

        //Gigamonkey::Merkle::path merkle_path(const Gigamonkey::digest<32> &digest) const;

        json header(const Gigamonkey::digest<32> &digest);
        json header(data::uint64 height);
        data::uint64 transaction_height(Gigamonkey::digest256 &txid);
        data::bytes raw_header(const Gigamonkey::digest<32> &digest) ;
        double price(Gigamonkey::Bitcoin::timestamp timestamp);
        //data::list<Gigamonkey::Bitcoin::txid> transactions(const Gigamonkey::digest<32> &digest) const;

        //data::bytes block(const Gigamonkey::digest<32> &digest) const ;
    private:
        mutable data::tools::rate_limiter rateLimit;
        mutable data::networking::Http http;
    };
}
#endif //COSMOSWALLET_MATTERPOOLAPI_H
