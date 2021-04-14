// Copyright (c) 2021 Katrina Knight
// Copyright (c) 2021 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOSWALLET_TYPES_H
#define COSMOSWALLET_TYPES_H

#include <cstdint>
#include <nlohmann/json.hpp>
#include <gigamonkey/timechain.hpp>

namespace Cosmos {
    
    using digest256 = Gigamonkey::digest256;
    using digest160 = Gigamonkey::digest160;
    
    using bytes = data::bytes;
    
    using string = std::string;
    
    using json = nlohmann::json;
    
    namespace Merkle {
        using namespace Gigamonkey::Merkle;
    }
    
    std::vector<char> HexToBytes(const string& hex);

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
            string coinbase_info;
            string coinbase_txid;
            char chain_work[32];

        };

    }

    struct BitcoinTransactions {

    };
}
#endif //COSMOSWALLET_TYPES_H
