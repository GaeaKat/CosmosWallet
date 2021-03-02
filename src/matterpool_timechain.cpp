// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "matterpool_timechain.h"

namespace Cosmos{

    data::list<Gigamonkey::uint<80>> MatterPool_TimeChain::headers(data::uint64 since_height) const {
        //auto vals=this->api.headers(since_height);
        std::string output=http.GET("txdb.mattercloud.io","/api/v1/blockheader/"+ std::to_string(since_height+(1000*0))+"?limit=1000&order=asc");
        //return data::list<Gigamonkey::uint<80>>();
    }

    data::bytes MatterPool_TimeChain::transaction(const Gigamonkey::digest<32> &digest) const {
        auto tmp=data::encoding::hex::write(digest,data::endian::order::little,data::encoding::hex::letter_case::lower);

        std::string output=http.GET("media.bitcoinfiles.org","/tx/"+ tmp+"/raw");
        //std::cout << output << std::endl;
        if(output=="{ }")
            return data::bytes();
        return data::bytes_view(data::encoding::hex::view{output});
    }

    Gigamonkey::Merkle::path MatterPool_TimeChain::merkle_path(const Gigamonkey::digest<32> &digest) const {
        return Gigamonkey::Merkle::path();
    }

    Gigamonkey::uint<80> MatterPool_TimeChain::header(const Gigamonkey::digest<32> &digest) const {
        return Gigamonkey::uint<80>();
    }

    data::list<Gigamonkey::Bitcoin::txid>
    MatterPool_TimeChain::transactions(const Gigamonkey::digest<32> &digest) const {
        return data::list<Gigamonkey::Bitcoin::txid>();
    }

    data::bytes MatterPool_TimeChain::block(const Gigamonkey::digest<32> &digest) const {
        return data::bytes();
    }
    void MatterPool_TimeChain::waitForRateLimit() const {
        long waitTime=rateLimit.getTime();
        if(waitTime > 0)
            sleep(waitTime);
    }
}