// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "matterpool_timechain.h"

namespace Cosmos::MatterPool {

    list<ledger::block_header> TimeChain::headers(uint64 since_height) {
        //auto vals=this->api.headers(since_height);
        list<ledger::block_header> headers=api.headers(since_height);
        for(auto header:headers) {
            db.insert(header);
        }
        return headers;
        //return list<Gigamonkey::uint<80>>();
    }

    data::entry<txid, ledger::double_entry> TimeChain::transaction(const txid &id) const {

        ledger::double_entry dentry;
        auto cache=db.get_transaction(id);
        if(cache.Key!=txid())
            return cache;
        auto trans=api.transaction(id);
        auto ptr=std::make_shared<bytes>(trans);
        auto height=api.transaction_height(const_cast<digest256 &>(id));
        auto header=this->header(height);
        if(header.valid()) {
            auto entry=data::entry<txid, ledger::double_entry>(id, ledger::double_entry(ptr, Merkle::proof(), header.Header));
            db.insert(entry);
            return entry;
        }
        auto tmp=data::entry<txid, ledger::double_entry>(id, ledger::double_entry());

        return tmp;

    }

    ledger::block_header TimeChain::header(const digest256 &digest) const {
        auto headerOut=db[digest];

        if(!headerOut.valid()) {
            json data = api.header(digest);
            MatterPool::Header header = data;
            //header(digest256 s, Bitcoin::header h, N n, work::difficulty d) : Hash{s}, Header{h}, Height{n}, Cumulative{d} {}
            string diffString;

            data["difficulty"].get_to(diffString);
            Gigamonkey::work::difficulty diff(stod(diffString));
            u_int32_t heightString;

            data["height"].get_to(heightString);

            data::math::number::gmp::N height(heightString);
            headerOut=ledger::block_header(digest, header, height, diff);

            db.insert(headerOut);
        }
        return headerOut;
    }


    void TimeChain::waitForRateLimit() const {
        long waitTime=rateLimit.getTime();
        if(waitTime > 0)
            sleep(waitTime);
    }

    bytes TimeChain::block(const digest256 &) const {
        return bytes();
    }

    bool TimeChain::broadcast(const data::bytes_view &) {
        return false;
    }

    ledger::block_header TimeChain::header(data::uint64 height) const {
        auto headerOut=db[height];

        if(!headerOut.valid()) {
            json data = api.header(height);
            MatterPool::Header header = data;
            //header(digest256 s, Bitcoin::header h, N n, work::difficulty d) : Hash{s}, Header{h}, Height{n}, Cumulative{d} {}
            string diffString;

            data["difficulty"].get_to(diffString);
            Gigamonkey::work::difficulty diff(stod(diffString));
            u_int32_t heightString;

            data["height"].get_to(heightString);

            data::math::number::gmp::N height(heightString);
            string hashString;
            data["hash"].get_to(hashString);
            digest256 digest("0x"+hashString);
            headerOut=ledger::block_header(digest, header, height, diff);

            db.insert(headerOut);
        }
        return headerOut;
    }

    list<data::entry<txid, ledger::double_entry>>
    TimeChain::transactions(const address &a) {
        auto txids=api.transactions(a);
        list<data::entry<txid, ledger::double_entry>> ret;
        for(json tx : txids) {
            ledger::double_entry dentry;
            string txString;
            tx["txid"].get_to(txString);
            digest256 id("0x"+txString);
            auto cache=db.get_transaction(id);
            
            if(cache.Key != txid()) {
                ret = ret << cache;
                continue;
            }
            
            auto trans=api.transaction(id);
            auto ptr=std::make_shared<bytes>(trans);
            uint64_t height;
            tx["height"].get_to(height);
            auto header=this->header(height);
            if(header.valid()) {
                auto entry=data::entry<txid, ledger::double_entry>(id, ledger::double_entry(ptr, Merkle::proof(), header.Header));
                db.insert(entry);
                ret = ret << entry;
            }

        }
        return ret;
    }
    
    double TimeChain::price(timestamp t) {
        time_t rawtime=static_cast<time_t>(uint32_t(t));
        struct tm * timeinfo;
        char buffer [11];
        timeinfo = localtime (&rawtime);

        strftime (buffer,11,"%d-%m-%Y",timeinfo);
        waitForRateLimit();
        string output;
        if(((long)t) > BSV_FORK_TIMESTAMP) {
            output = http->GET("api.coingecko.com",
                                    "/api/v3/coins/bitcoin-cash-sv/history?date=" + string(buffer));
        } else if(((long)t) > CASH_FORK_TIMESTAMP) {
            output = http->GET("api.coingecko.com",
                                    "/api/v3/coins/bitcoin-cash/history?date=" + string(buffer));
        }
        else {
            // bitcoin
            output = http->GET("api.coingecko.com",
                                    "/api/v3/coins/bitcoin/history?date=" + string(buffer));
        }
        try {
            json jOutput = json::parse(output);
            return jOutput["market_data"]["current_price"]["usd"];
        }
        catch(std::exception e) {
            return 0;
        }
    }
    
}
