// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "types.h"
#include <data/tools/rate_limiter.h>
#include <data/types.hpp>
#include <data/networking/http.h>
#include <gigamonkey/hash.hpp>
#include <gigamonkey/ledger.hpp>
#include <gigamonkey/address.hpp>

#ifndef COSMOSWALLET_MATTERPOOLAPI_H
#define COSMOSWALLET_MATTERPOOLAPI_H

namespace Cosmos::MatterPool {
    
    struct Header : Gigamonkey::Bitcoin::header {};
    
    void from_json(const json& j, Header& header);
    void to_json(json& j, const Header& header);

    class Api {
    public:
        
        Api() : rateLimit(100,60) {}
        data::list<Gigamonkey::Bitcoin::ledger::block_header> headers(data::uint64 since_height) ;

        data::bytes transaction(const digest256 &digest) ;
        json transactions(const Gigamonkey::Bitcoin::address address);
        //Gigamonkey::Merkle::path merkle_path(const digest256 &digest) const;

        json header(const digest256 &digest);
        json header(data::uint64 height);
        data::uint64 transaction_height(digest256 &txid);
        data::bytes raw_header(const digest256 &digest);
        
        static Gigamonkey::Bitcoin::header header_from_json();

        //data::list<txid> transactions(const digest256 &digest) const;

        //data::bytes block(const digest256 &digest) const ;
        
    private:
        mutable data::tools::rate_limiter rateLimit;
        mutable data::networking::Http http;
        
        void waitForRateLimit();
    };
}
#endif //COSMOSWALLET_MATTERPOOLAPI_H
