// Copyright (c) 2021 Katrina Knight
// Copyright (c) 2021 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOSWALLET_TYPES_H
#define COSMOSWALLET_TYPES_H

#include <cstdint>
#include <nlohmann/json.hpp>
#include <gigamonkey/timechain.hpp>
#include <gigamonkey/boost/boost.hpp>

namespace Cosmos {
    using namespace Gigamonkey::Bitcoin;
    
    using uint64 = data::uint64;
    using uint32 = data::uint32;
    
    using digest256 = Gigamonkey::digest256;
    using digest160 = Gigamonkey::digest160;
    
    using bytes = data::bytes;
    
    using string = std::string;
    
    using json = nlohmann::json;
    
    template <typename X> using list = data::list<X>;
    
    using N = Gigamonkey::N;
    
    template <typename X> using ptr = std::shared_ptr<X>;
    
    namespace Merkle {
        using namespace Gigamonkey::Merkle;
    }
    
    namespace Boost {
        using namespace Gigamonkey::Boost;
    }
    
    constexpr int satoshi_dust{546};
    
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
