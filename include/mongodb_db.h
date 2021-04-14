//
// Created by nekosune on 27/03/2021.
//

#ifndef COSMOSWALLET_MONGODB_DB_H
#define COSMOSWALLET_MONGODB_DB_H

#include <gigamonkey/spv.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <gigamonkey/ledger.hpp>

#include "types.h"

namespace Cosmos::Mongo {

    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::close_document;
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::finalize;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::open_document;

    class DB : Gigamonkey::Bitcoin::headers {
    private:
        static mongocxx::instance instance;
        static bool initialized;
        static mongocxx::client client;
        static mongocxx::database db;
        static mongocxx::collection headers_collection;
        static mongocxx::collection transaction_collection;
        static void initialize();
    protected:
        const bsoncxx::document::value to_document(header header);
        header from_document(bsoncxx::document::value document) const;
        const bsoncxx::document::value transaction_to_document(data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> transaction);
        [[nodiscard]] data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> transaction_from_document(bsoncxx::document::value document) const;
    public:
        [[nodiscard]] header latest() const override;
        data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> get_transaction(const digest256 &digest);
        header operator[](const N &n) const override;
        header operator[](const digest256 &digest) const override;

        [[nodiscard]] Merkle::dual dual_tree(const digest256 &digest) const override;

        [[nodiscard]] Merkle::proof proof(const Gigamonkey::Bitcoin::txid &txid) const override;

        bool insert(const header &header) override;
        bool insert(const data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> &transaction);

        bool insert(const Merkle::proof &proof) override;
    };
}
#endif //COSMOSWALLET_MONGODB_DB_H
