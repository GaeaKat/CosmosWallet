#include <hd_wallet.h>


namespace Cosmos {
    
    void hd_wallet::populate_account(int32_t account_index, const uint32 max_unused_before_quit) {
        if (account_index < 0) return;
        
        account new_account{};
        
        new_account.Master = Master.account(account_index);
        
        for (const uint32 path : data::list<uint32>{0, 1}) {
            
            auto key = new_account.Master.derive(list<uint32>{path});
            
            address_chain chain {-1, {}};
        
            int address_index = 0;
            
            while(true) {
                
                address bitcoin_address = key.derive(address_index).to_public().address();
                
                auto transactions_by_address = TimeChain.transactions(bitcoin_address);
                
                std::cout << transactions_by_address.size() << " transactions found: " << transactions_by_address << std::endl;
                
                if (transactions_by_address.size() == 0) {
                    if (address_index - chain.LastIndex > max_unused_before_quit) break;
                } else chain.LastIndex = address_index;
                
                list<txid> associated_txids{};
                
                // for each of these txs, which outputs are associated with these addresses ? 
                
                for (const auto x : transactions_by_address) {
                    
                    if (x.Value.id() != x.Key) {
                        std::cout << "warning: " << x.Key << " returns a tx with hash " << x.Value.id() << std::endl;
                        //wait_for_enter();
                        throw 0;
                    } 
                    
                    new_account.Transactions = new_account.Transactions.insert(x.Key, TimeChain.make_vertex(x.Value));
                    associated_txids = associated_txids << x.Key;
                    
                }
                
                if (!associated_txids.empty()) {
                    chain.Addresses = chain.Addresses.insert(address_index, bitcoin_address.Digest);
                    new_account.Addresses = new_account.Addresses.insert(bitcoin_address.Digest, associated_txids);
                }
                
                address_index++;
                
            }
            
            new_account.Chains = new_account.Chains.insert(path, chain);
            
        }
        
        Accounts[account_index] = new_account;
        
    }
    
}
