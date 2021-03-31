// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "matterpool_timechain.h"

namespace Cosmos{

    data::list<Gigamonkey::Bitcoin::ledger::block_header> MatterPool_TimeChain::headers(data::uint64 since_height) const {
        //auto vals=this->api.headers(since_height);
        std::string output=http.GET("txdb.mattercloud.io","/api/v1/blockheader/"+ std::to_string(since_height+(1000*0))+"?limit=1000&order=asc");
        //return data::list<Gigamonkey::uint<80>>();
    }

    data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> MatterPool_TimeChain::transaction(const Gigamonkey::digest<32> &txid) const {
        Gigamonkey::Bitcoin::ledger::double_entry dentry;
        auto trans=api.transaction(txid);
        auto ptr=std::make_shared<Gigamonkey::bytes>(trans);
        auto height=api.transaction_height(const_cast<Gigamonkey::digest256 &>(txid));
        auto header=this->header(height);
        if(header.valid()) {
            auto entry=data::entry<Gigamonkey::Bitcoin::txid,Gigamonkey::Bitcoin::ledger::double_entry>(txid, Gigamonkey::Bitcoin::ledger::double_entry(ptr, Gigamonkey::Merkle::proof(), header.Header));
            return entry;
        }
        return data::entry<Gigamonkey::Bitcoin::txid,Gigamonkey::Bitcoin::ledger::double_entry>(txid, Gigamonkey::Bitcoin::ledger::double_entry());

    }



    Gigamonkey::Bitcoin::ledger::block_header MatterPool_TimeChain::header(const Gigamonkey::digest<32> &digest) const {
        auto headerOut=db[digest];

        if(!headerOut.valid()) {
            json data = api.header(digest);
            Gigamonkey::Bitcoin::header header = data;
            //header(digest256 s, Bitcoin::header h, N n, work::difficulty d) : Hash{s}, Header{h}, Height{n}, Cumulative{d} {}
            std::string diffString;

            data["difficulty"].get_to(diffString);
            Gigamonkey::work::difficulty diff(stod(diffString));
            u_int32_t heightString;

            data["height"].get_to(heightString);

            data::math::number::gmp::N height(heightString);
            headerOut=Gigamonkey::Bitcoin::ledger::block_header(digest, header, height, diff);

            db.insert(headerOut);
        }
        return headerOut;
    }


    void MatterPool_TimeChain::waitForRateLimit() const {
        long waitTime=rateLimit.getTime();
        if(waitTime > 0)
            sleep(waitTime);
    }

    Gigamonkey::bytes MatterPool_TimeChain::block(const Gigamonkey::digest256 &) const {
        return Gigamonkey::bytes();
    }

    bool MatterPool_TimeChain::broadcast(const data::bytes_view &) {
        return false;
    }

    Gigamonkey::Bitcoin::ledger::block_header MatterPool_TimeChain::header(data::uint64 height) const {
        auto headerOut=db[height];

        if(!headerOut.valid()) {
            json data = api.header(height);
            Gigamonkey::Bitcoin::header header = data;
            //header(digest256 s, Bitcoin::header h, N n, work::difficulty d) : Hash{s}, Header{h}, Height{n}, Cumulative{d} {}
            std::string diffString;

            data["difficulty"].get_to(diffString);
            Gigamonkey::work::difficulty diff(stod(diffString));
            u_int32_t heightString;

            data["height"].get_to(heightString);

            data::math::number::gmp::N height(heightString);
            std::string hashString;
            data["hash"].get_to(hashString);
            Gigamonkey::digest256 digest("0x"+hashString);
            headerOut=Gigamonkey::Bitcoin::ledger::block_header(digest, header, height, diff);

            db.insert(headerOut);
        }
        return headerOut;
    }
}
