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
    class TimeChain: public Gigamonkey::Bitcoin::timechain {
    public:
        TimeChain(): rateLimit(100,60) {}
        double price(Gigamonkey::Bitcoin::timestamp timestamp);

        data::list<Gigamonkey::Bitcoin::ledger::block_header> headers(data::uint64 since_height) const override;

        data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> transaction(const digest256 &txid) const override;
        Gigamonkey::Bitcoin::ledger::block_header header(const digest256 &digest) const override;
        data::list<data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry>> transactions(const Gigamonkey::Bitcoin::address address);
    private:
        Gigamonkey::Bitcoin::ledger::block_header header(data::uint64 height) const ;
        void waitForRateLimit() const;
        //Gigamonkey::Merkle::path merkle_path(const digest256 &digest) const override;



        //data::list<Gigamonkey::Bitcoin::txid> transactions(const digest256 &digest) const override;

        Gigamonkey::bytes block(const digest256&) const override;
        bool broadcast(const Gigamonkey::bytes_view&) override;
    private:
        mutable data::tools::rate_limiter rateLimit;
        mutable data::networking::Http http;
        mutable MatterPool::Api api;
        mutable MongoDB_DB db;

    };
}
#endif //COSMOSWALLET_MATTERPOOL_TIMECHAIN_H
