// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <data/types.hpp>
#include "MatterpoolAPI.h"

namespace Cosmos {

    std::vector<BitcoinHeader::BitcoinHeader> MatterpoolApi::headers(data::uint64 since_height){
        auto ret=std::vector<BitcoinHeader::BitcoinHeader>();
        int i=0;
        json jOutput;
        do {
            waitForRateLimit();
            std::string output=this->http.GET("txdb.mattercloud.io","/api/v1/blockheader/"+ std::to_string(since_height+(1000*i))+"?limit=1000&order=asc");
            i++;
            jOutput=json::parse(output);
            for(BitcoinHeader::BitcoinHeader header : jOutput["result"]) {
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
}