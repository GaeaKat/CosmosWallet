// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.



#include <types.h>
#include <data/encoding/hex.hpp>
#include <sstream>
#include <string>     // std::string, std::stod
#include <iomanip>
void Gigamonkey::Bitcoin::from_json(const json &j, header &header) {
    uint32 version;
    j.at("version").get_to(version);
    header.Version=version;
    std::string merkle;
    j.at("merkleroot").get_to(merkle);
    digest256 tmp("0x"+merkle);
    header.MerkleRoot=tmp;
    uint32 nonce;
    j.at("nonce").get_to(nonce);
    header.Nonce=nonce;
    std::string previous;
    j.at("previousblockhash").get_to(previous);
    digest256 previousTmp("0x"+previous);
    header.Previous=previousTmp;
    uint32 tmpTimestamp;
    j.at("time").get_to(tmpTimestamp);

    header.Timestamp=Gigamonkey::Bitcoin::timestamp(tmpTimestamp);
    std::string target;
    j.at("bits").get_to(target);
    unsigned int x;
    std::stringstream ss;
    ss << std::hex << target;
    ss >> x;
    header.Target=Gigamonkey::work::compact(x);

    /*j.at("height").get_to(header.height);

    std::string hash;
    j.at("hash").get_to(hash);
    std::vector<char> hashBytes=HexToBytes(hash);
    memcpy(header.hash,&hashBytes[0],32);
    j.at("size").get_to(header.size);
    j.at("version").get_to(header.version);
    std::string merkle;
    j.at("merkleroot").get_to(merkle);
    std::vector<char> merkleBytes=HexToBytes(merkle);
    memcpy(header.merkle_root, &merkleBytes[0], 32);
    j.at("time").get_to(header.timestamp);
    j.at("nonce").get_to(header.nonce);
    std::string bits;
    j.at("bits").get_to(bits);
    std::istringstream converter(bits);
    converter >> std::hex >> header.bits;
    std::string difficulty;
    j.at("difficulty").get_to(difficulty);
    header.difficulty = std::stold(difficulty.c_str());
    std::string previous;
    j.at("previousblockhash").get_to(previous);
    std::vector<char> previousBytes=HexToBytes(previous);
    memcpy(header.prev_block,&previousBytes[0],32);
    std::string next;
    j.at("nextblockhash").get_to(next);
    if(!next.empty()) {
        std::vector<char> nextBytes = HexToBytes(next);
        memcpy(header.next_block, &nextBytes[0], 32);
    }
    j.at("coinbaseinfo").get_to(header.coinbase_info);
    j.at("coinbasetxid").get_to(header.coinbase_txid);
    std::string chain_work;
    j.at("chainwork").get_to(chain_work);
    std::vector<char> chainBytes =HexToBytes(chain_work);
    memcpy(header.chain_work,&chainBytes[0],32);*/



}

void Gigamonkey::Bitcoin::to_json(json &j, const Gigamonkey::Bitcoin::header &header) {

}

std::vector<char> Cosmos::HexToBytes(const std::string &hex) {
    std::vector<char> bytes;

    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = (char) strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }

    return bytes;
}
