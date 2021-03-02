// Copyright (c) 2021 Katrina Knight
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOSWALLET_TYPES_H
#define COSMOSWALLET_TYPES_H

#include <cstdint>
#include <nlohmann/json.hpp>

using nlohmann::json;
namespace Cosmos {
    std::vector<char> HexToBytes(const std::string& hex);

    namespace BitcoinHeader {
        struct BitcoinHeader {
            uint32_t height;
            char hash[32];
            uint32_t size;
            int32_t version;
            char merkle_root[32];
            uint32_t timestamp;
            uint32_t nonce;
            uint32_t bits;
            long double difficulty;
            char prev_block[32];
            char next_block[32];
            std::string coinbase_info;
            std::string coinbase_txid;
            char chain_work[32];

        };
        void from_json(const json& j, BitcoinHeader& header);
        void to_json(json& j, const BitcoinHeader& header);


    }

    struct BitcoinTransactions {

    };
}
#endif //COSMOSWALLET_TYPES_H
