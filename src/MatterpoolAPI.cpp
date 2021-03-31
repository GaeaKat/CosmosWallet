// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <data/types.hpp>
#include "MatterpoolAPI.h"
#include "types.h"

namespace Cosmos {

    std::vector<Gigamonkey::Bitcoin::header> MatterpoolApi::headers(data::uint64 since_height){
        auto ret=std::vector<Gigamonkey::Bitcoin::header>();
        int i=0;
        json jOutput;
        do {
            waitForRateLimit();
            std::string output=this->http.GET("txdb.mattercloud.io","/api/v1/blockheader/"+ std::to_string(since_height+(1000*i))+"?limit=1000&order=asc");
            i++;
            jOutput=json::parse(output);
            for(Gigamonkey::Bitcoin::header header : jOutput["result"]) {
                ret.push_back(header);
            }

        }while(jOutput["result"].size()==1000);

        return ret;
    }

    void MatterpoolApi::waitForRateLimit() {
        long waitTime=rateLimit.getTime();
        if(waitTime > 0)
            sleep(waitTime);
    }

    data::bytes MatterpoolApi::transaction(const Gigamonkey::digest<32> &digest)   {
        auto tmp=data::encoding::hex::write(digest,data::endian::order::little,data::encoding::hex::letter_case::lower);
        waitForRateLimit();
        std::string output=http.GET("media.bitcoinfiles.org","/tx/"+ tmp+"/raw");
        //std::cout << output << std::endl;
        if(output=="{}")
            return data::bytes();
        return data::bytes_view(data::encoding::hex::view{output});
    }

    json MatterpoolApi::header(const Gigamonkey::digest<32> &digest) {
        auto tmp=data::encoding::hex::write(digest,data::endian::order::little,data::encoding::hex::letter_case::lower);
        waitForRateLimit();
        std::string output=this->http.GET("txdb.mattercloud.io","/api/v1/blockheader/"+ tmp+"?limit=1&order=asc");
        json jOutput=json::parse(output);
        return jOutput["result"][0];
    }

    data::bytes MatterpoolApi::raw_header(const Gigamonkey::digest<32> &digest) {
        auto tmp=data::encoding::hex::write(digest,data::endian::order::little,data::encoding::hex::letter_case::lower);
        waitForRateLimit();
        std::string output=http.GET("media.bitcoinfiles.org","/rawblockheader/"+ tmp);
        //std::cout << output << std::endl;
        //if(output=="{ }")
        //    return data::bytes();
        return data::bytes_view(data::encoding::hex::view{output});
    }

    json MatterpoolApi::header(data::uint64 height) {

        waitForRateLimit();
        std::string output=this->http.GET("txdb.mattercloud.io","/api/v1/blockheader/"+ std::to_string(height)+"?limit=1&order=asc");
        json jOutput=json::parse(output);
        return jOutput["result"][0];
    }

    data::uint64 MatterpoolApi::transaction_height(Gigamonkey::digest256 &txid) {
        auto tmp=data::encoding::hex::write(txid,data::endian::order::little,data::encoding::hex::letter_case::lower);
        waitForRateLimit();
        std::string output=this->http.GET("txdb.mattercloud.io","/api/v1/txblock/"+ tmp);
        json jOutput=json::parse(output);
        return jOutput["result"][0]["height"];
    }

    double MatterpoolApi::price(Gigamonkey::Bitcoin::timestamp timestamp) {
        time_t rawtime=static_cast<time_t>(uint32_t(timestamp));
        struct tm * timeinfo;
        char buffer [11];
        timeinfo = localtime (&rawtime);

        strftime (buffer,11,"%d-%m-%Y",timeinfo);
        waitForRateLimit();
        std::string output=this->http.GET("api.coingecko.com","/api/v3/coins/bitcoin-cash-sv/history?date="+std::string(buffer));
        json jOutput=json::parse(output);
        return jOutput["market_data"]["current_price"]["usd"];
    }
}