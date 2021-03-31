//
// Created by nekosune on 27/03/2021.
//

#ifndef COSMOSWALLET_MONGODB_HEADERS_H
#define COSMOSWALLET_MONGODB_HEADERS_H

#include <gigamonkey/spv.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>


using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

namespace Cosmos {
    class MongoDB_Headers : Gigamonkey::Bitcoin::headers {
    private:
        static mongocxx::instance instance;
        static bool initialized;
        static mongocxx::client client;
        static mongocxx::database db;
        static mongocxx::collection headers_collection;
        static void initialize();
    protected:
        const bsoncxx::document::value to_document(header header);
        header from_document(bsoncxx::document::value document) const;
    public:
        header latest() const override;

        header operator[](const Gigamonkey::N &n) const override;
        header operator[](const Gigamonkey::digest256 &digest) const override;

        Gigamonkey::Merkle::dual dual_tree(const Gigamonkey::digest256 &digest256) const override;

        Gigamonkey::Merkle::proof proof(const Gigamonkey::Bitcoin::txid &txid) const override;

        bool insert(const header &header) override;

        bool insert(const Gigamonkey::Merkle::proof &proof) override;
    };
}
#endif //COSMOSWALLET_MONGODB_HEADERS_H
