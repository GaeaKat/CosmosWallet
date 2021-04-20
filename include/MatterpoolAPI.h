// Copyright (c) 2021 Katrina Knight
// Copyright (c) 2021 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "types.h"
#include <data/tools/rate_limiter.h>
#include <data/types.hpp>
#include <data/networking/http.hpp>
#include <gigamonkey/hash.hpp>
#include <gigamonkey/ledger.hpp>
#include <gigamonkey/address.hpp>
#include <gigamonkey/mapi/mapi.hpp>

#ifndef COSMOSWALLET_MATTERPOOLAPI_H
#define COSMOSWALLET_MATTERPOOLAPI_H

namespace Cosmos::MatterPool {
    
    struct Header : header {};
    
    void from_json(const json&, Header&);
    void to_json(json&, const Header&);

    class Api {
    public:
        
        Api(ptr<data::networking::http> p) : rateLimit(100,60), http(p) {}
        list<ledger::block_header> headers(uint64 since_height) ;

        bytes transaction(const digest256 &);
        json transactions(const address &);
        //Merkle::path merkle_path(const digest256 &digest) const;

        json header(const digest256 &);
        json header(uint64 height);
        
        uint64 transaction_height(digest256 &txid);
        bytes raw_header(const digest256 &digest);

        //list<txid> transactions(const digest256 &digest) const;

        //bytes block(const digest256 &digest) const ;
        
        Gigamonkey::merchant_api mapi() {
            return Gigamonkey::merchant_api{"merchantapi.matterpool.io", http};
        }
        
    private:
        data::tools::rate_limiter rateLimit;
        ptr<data::networking::http> http;
        
        void waitForRateLimit();
    };
}
#endif //COSMOSWALLET_MATTERPOOLAPI_H
