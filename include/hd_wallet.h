// Copyright (c) 2021 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "matterpool_timechain.h"
#include <gigamonkey/wallet.hpp>
#include <gigamonkey/schema/hd.hpp>

#ifndef COSMOSWALLET_HD_WALLET_H
#define COSMOSWALLET_HD_WALLET_H

namespace Cosmos {
    
    struct hd_wallet {
        
        struct address_chain {
            int32_t LastIndex;
            data::map<uint32, digest160> Addresses;
            
            address_chain() : LastIndex{-1}, Addresses{} {}
            address_chain(int32_t last, data::map<uint32, digest160> addr) : LastIndex{last}, Addresses{addr} {}
            
            bool operator==(const address_chain& ad) const {
                return LastIndex == ad.LastIndex && Addresses == ad.Addresses;
            }
            
            bool operator!=(const address_chain& ad) const {
                return !(*this == ad);
            }
        };
        
        struct account {
            hd::bip32::secret Master;
            wallet Wallet;
            
            data::map<txid, ledger::vertex> Transactions;
            
            data::map<digest160, list<txid>> Addresses;
            data::map<uint32, address_chain> Chains;
        };
        
        MatterPool::TimeChain& TimeChain;
        hd::bip44::secret Master;
        std::map<int32_t, account> Accounts;
        data::map<txid, ledger::vertex> Transactions;
        
        hd_wallet(MatterPool::TimeChain &t, const hd::bip44::secret& key) : TimeChain{t}, Master{key} {}
        
        void populate_account(int32_t account_index = 0, const uint32 max_unused_before_quit = 10);
        
    };
    
}

#endif
