// Copyright (c) 2021 Katrina Knight
// Copyright (c) 2021 Daniel Krawisz
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
    
    struct Header : header {};
    
    void from_json(const json&, Header&);
    void to_json(json&, const Header&);

    class Api {
    public:
        
        Api() : rateLimit(100,60) {}
        list<ledger::block_header> headers(uint64 since_height) ;

        bytes transaction(const digest256 &) ;
        json transactions(const address &);
        //Merkle::path merkle_path(const digest256 &digest) const;

        json header(const digest256 &);
        json header(uint64 height);
        
        uint64 transaction_height(digest256 &txid);
        bytes raw_header(const digest256 &digest);

        //list<txid> transactions(const digest256 &digest) const;

        //bytes block(const digest256 &digest) const ;
        
    private:
        mutable data::tools::rate_limiter rateLimit;
        mutable data::networking::Http http;
        
        void waitForRateLimit();
    };
}
#endif //COSMOSWALLET_MATTERPOOLAPI_H
