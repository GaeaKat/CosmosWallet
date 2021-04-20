// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <gigamonkey/ledger.hpp>
#include "MatterpoolAPI.h"
#include "mongodb_db.h"

#ifndef COSMOSWALLET_MATTERPOOL_TIMECHAIN_H
#define COSMOSWALLET_MATTERPOOL_TIMECHAIN_H

namespace Cosmos::MatterPool {
    constexpr long BSV_FORK_TIMESTAMP = 1542304320;
    constexpr long CASH_FORK_TIMESTAMP = 1501593373;
    
    class TimeChain: public timechain {
    public:
        TimeChain(ptr<data::networking::http> p): rateLimit(100,60), http(p), api(p) {}
        double price(timestamp timestamp);

        list<ledger::block_header> headers(uint64 since_height) override;

        data::entry<txid, ledger::double_entry> transaction(const txid &txid) const override;
        ledger::block_header header(const digest256 &digest) const override;
        list<data::entry<txid, ledger::double_entry>> transactions(const address&);
    private:
        ledger::block_header header(data::uint64 height) const ;
        void waitForRateLimit() const;
        //Merkle::path merkle_path(const digest256 &digest) const override;

        //list<txid> transactions(const digest256 &digest) const override;

        bytes block(const digest256&) const override;
        bool broadcast(const data::bytes_view&) override;
        
    private:
        mutable data::tools::rate_limiter rateLimit;
        mutable ptr<data::networking::http> http;
        mutable MatterPool::Api api;
        mutable Mongo::DB db;

    };
}
#endif //COSMOSWALLET_MATTERPOOL_TIMECHAIN_H
