// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <data/types.hpp>
#include "MatterpoolAPI.h"
#include "types.h"

namespace Cosmos::MatterPool {
    
    void from_json(const json &j, Header &h) {
        
        uint32 version;
        j.at("version").get_to(version);
        h.Version=version;
        string merkle;
        j.at("merkleroot").get_to(merkle);
        digest256 tmp("0x"+merkle);
        h.MerkleRoot=tmp;
        uint32 nonce;
        j.at("nonce").get_to(nonce);
        h.Nonce=nonce;
        string previous;
        j.at("previousblockhash").get_to(previous);
        digest256 previousTmp("0x"+previous);
        h.Previous=previousTmp;
        uint32 tmpTimestamp;
        j.at("time").get_to(tmpTimestamp);

        h.Timestamp=timestamp(tmpTimestamp);
        string target;
        j.at("bits").get_to(target);
        unsigned int x;
        std::stringstream ss;
        ss << std::hex << target;
        ss >> x;
        h.Target=Gigamonkey::work::compact(x);

        /*j.at("height").get_to(header.height);

        string hash;
        j.at("hash").get_to(hash);
        std::vector<char> hashBytes=HexToBytes(hash);
        memcpy(header.hash,&hashBytes[0],32);
        j.at("size").get_to(header.size);
        j.at("version").get_to(header.version);
        string merkle;
        j.at("merkleroot").get_to(merkle);
        std::vector<char> merkleBytes=HexToBytes(merkle);
        memcpy(header.merkle_root, &merkleBytes[0], 32);
        j.at("time").get_to(header.timestamp);
        j.at("nonce").get_to(header.nonce);
        ]string bits;
        j.at("bits").get_to(bits);
        std::istringstream converter(bits);
        converter >> std::hex >> header.bits;
        string difficulty;
        j.at("difficulty").get_to(difficulty);
        header.difficulty = std::stold(difficulty.c_str());
        string previous;
        j.at("previousblockhash").get_to(previous);
        std::vector<char> previousBytes=HexToBytes(previous);
        memcpy(header.prev_block,&previousBytes[0],32);
        string next;
        j.at("nextblockhash").get_to(next);
        if(!next.empty()) {
            std::vector<char> nextBytes = HexToBytes(next);
            memcpy(header.next_block, &nextBytes[0], 32);
        }
        j.at("coinbaseinfo").get_to(header.coinbase_info);
        j.at("coinbasetxid").get_to(header.coinbase_txid);
        string chain_work;
        j.at("chainwork").get_to(chain_work);
        std::vector<char> chainBytes =HexToBytes(chain_work);
        memcpy(header.chain_work,&chainBytes[0],32);*/



    }

    list<ledger::block_header> Api::headers(data::uint64 since_height){
        auto ret=list<headers::header>();
        int i=0;
        json jOutput;
        do {
            waitForRateLimit();
            string output=http->GET("txdb.mattercloud.io","/api/v1/blockheader/"+ std::to_string(since_height+(1000*i))+"?limit=1000&order=asc");
            i++;
            jOutput=json::parse(output);
            for(json headerData : jOutput["result"]) {
                Header header = headerData;
                //header(digest256 s, Bitcoin::header h, N n, work::difficulty d) : Hash{s}, Header{h}, Height{n}, Cumulative{d} {}
                string diffString;

                headerData["difficulty"].get_to(diffString);
                Gigamonkey::work::difficulty diff(stod(diffString));
                u_int32_t heightString;

                headerData["height"].get_to(heightString);

                N height(heightString);
                string hashString;
                headerData["hash"].get_to(hashString);
                digest256 digest("0x"+hashString);
                auto headerOut=ledger::block_header(digest, header, height, diff);
                ret=ret << headerOut;
            }

        }while(jOutput["result"].size()==1000);

        return ret;
    }

    void Api::waitForRateLimit() {
        long waitTime=rateLimit.getTime();
        if(waitTime > 0)
            sleep(waitTime);
    }

    bytes Api::transaction(const digest256 &digest)   {
        auto tmp=data::encoding::hex::write(digest,data::encoding::hex::letter_case::lower);
        waitForRateLimit();
        string output=http->GET("media.bitcoinfiles.org","/tx/"+ tmp+"/raw");
        ptr<bytes> hex = data::encoding::hex::read(output);
        return hex == nullptr ? bytes{} : *hex;
    }

    json Api::header(const digest256 &digest) {
        auto tmp=data::encoding::hex::write(digest,data::encoding::hex::letter_case::lower);
        waitForRateLimit();
        string output=http->GET("txdb.mattercloud.io","/api/v1/blockheader/"+ tmp+"?limit=1&order=asc");
        json jOutput=json::parse(output);
        return jOutput["result"][0];
    }

    bytes Api::raw_header(const digest256 &digest) {
        auto tmp=data::encoding::hex::write(digest,data::encoding::hex::letter_case::lower);
        waitForRateLimit();
        string output=http->GET("media.bitcoinfiles.org","/rawblockheader/"+ tmp);
        ptr<bytes> hex = data::encoding::hex::read(output);
        return hex == nullptr ? bytes{} : *hex;
    }

    json Api::header(uint64 height) {

        waitForRateLimit();
        string output=http->GET("txdb.mattercloud.io","/api/v1/blockheader/"+ std::to_string(height)+"?limit=1&order=asc");
        json jOutput=json::parse(output);
        return jOutput["result"][0];
    }

    uint64 Api::transaction_height(digest256 &txid) {
        auto tmp=data::encoding::hex::write(txid,data::encoding::hex::letter_case::lower);
        waitForRateLimit();
        string output=http->GET("txdb.mattercloud.io","/api/v1/txblock/"+ tmp);
        json jOutput=json::parse(output);
        if(jOutput["result"].empty())
            return -1;
        return jOutput["result"][0]["height"];
    }

    json Api::transactions(const address &a) {
        waitForRateLimit();
        string output=http->GET("txdb.mattercloud.io","/api/v1/txout/address/history/"+a.write());
        json jOutput=json::parse(output);
        return jOutput["result"];
    }
}
