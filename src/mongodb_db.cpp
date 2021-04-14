// Copyright (c) 2021 Katrina Knight
// Copyright (c) 2021 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "mongodb_db.h"
#include "types.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::make_array;

namespace Gigamonkey {
    namespace Bitcoin {
        headers::header::header() {

        }
    }
}

namespace Cosmos::Mongo {
    bool DB::initialized;
    mongocxx::instance DB::instance{};
    mongocxx::client DB::client;
    mongocxx::database DB::db;
    mongocxx::collection DB::headers_collection;
    mongocxx::collection DB::transaction_collection;

    Gigamonkey::Bitcoin::headers::header DB::latest() const {
        return Gigamonkey::Bitcoin::headers::header();
    }

    Gigamonkey::Bitcoin::headers::header DB::operator[](const N &n) const {
        initialize();
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                headers_collection.find_one(document{} << "Height" << (double)n << finalize);
        if(maybe_result) {
                auto doc=maybe_result.value();
                auto header= from_document(doc);
                return header;
        }
        else {
            return Gigamonkey::Bitcoin::headers::header();
        }
    }

    Merkle::dual DB::dual_tree(const digest256 &digest) const {
        return Merkle::dual();
    }

    Merkle::proof DB::proof(const Gigamonkey::Bitcoin::txid &txid) const {
        return Merkle::proof();
    }

    bool DB::insert(const header &header) {
        initialize();
        auto doc=to_document(header);
        try {
            bsoncxx::stdx::optional<mongocxx::result::insert_one> result = headers_collection.insert_one(
                    std::move(doc));
            return (bool)result;
        }
        catch(std::exception e) {

        }

        return false;
    }

    bool DB::insert(const Merkle::proof &proof) {
        return false;
    }

    void DB::initialize() {
        if(initialized)
            return;

        DB::client=mongocxx::client(mongocxx::uri{});
        DB::db = client["Cosmos"];
        DB::headers_collection = db["headers"];
        DB::transaction_collection=db["transactions"];

        mongocxx::options::index index_options{};
        index_options.unique(true);
        // Header indexes
        int header_index_count=std::distance(headers_collection.indexes().list().begin(), headers_collection.indexes().list().end());
        if(header_index_count !=2) {
            headers_collection.create_index(make_document(kvp("Height", 1), kvp("Hash", 1)),
                                                             index_options);
        }
        // Transaction index
        int transaction_index_count=std::distance(transaction_collection.indexes().list().begin(), transaction_collection.indexes().list().end());
        if(transaction_index_count == 0) {
            transaction_collection.create_index(make_document(kvp("Hash", 1)),
                                                        index_options);
        }
        std::cout << "Transaction index: " << transaction_index_count << std::endl;

        initialized = true;
    }

    const bsoncxx::document::value DB::to_document(Gigamonkey::Bitcoin::headers::header header) {
        auto builder = bsoncxx::builder::stream::document{};
        auto hashString=data::encoding::hex::write(header.Hash, data::endian::order::little, data::encoding::hex::letter_case::lower);
        auto previousString=data::encoding::hex::write(header.Header.Previous, data::endian::order::little, data::encoding::hex::letter_case::lower);
        auto markleString=data::encoding::hex::write(header.Header.MerkleRoot, data::endian::order::little, data::encoding::hex::letter_case::lower);
        //bsoncxx::document::value doc_value =make_document(
        //        kvp("Hash",hashString)
        //        );
        bsoncxx::document::value doc_value = builder
                << "Hash" << hashString
                << "Height" << (double) header.Height
                << "Difficulty" << header.Cumulative.Value
                << "Version" << (int) header.Header.Version
                << "Previous" << previousString
                << "MerkleRoot" << markleString
                << "Timestamp" << (long) header.Header.Timestamp
                << "Target" << static_cast<int64_t>(header.Header.Target)
                << "Nonce" << static_cast<int64_t>(header.Header.Nonce)
                << bsoncxx::builder::stream::finalize;
        return doc_value;
    }

    Gigamonkey::Bitcoin::headers::header DB::from_document(bsoncxx::document::value document) const {
        Gigamonkey::Bitcoin::header header;
        auto doc=document.view();
        string previousString=doc["Previous"].get_utf8().value.to_string();
        header.Previous = digest256 ("0x"+previousString);
        header.Version = doc["Version"].get_int32().value;
        string markleString=doc["MerkleRoot"].get_utf8().value.to_string();
        header.MerkleRoot = digest256 ("0x"+markleString);
        header.Timestamp = Gigamonkey::Bitcoin::timestamp(doc["Timestamp"].get_int64().value);
        header.Target = Gigamonkey::Bitcoin::target(static_cast<uint32_t>(doc["Target"].get_int64().value));
        header.Nonce = static_cast<uint32_t>(doc["Nonce"].get_int64().value);
        string hashString = doc["Hash"].get_utf8().value.to_string();
        digest256 hashDigest("0x"+hashString);
        Gigamonkey::N  Height = Gigamonkey::N(doc["Height"].get_double().value);
        Gigamonkey::work::difficulty diff=Gigamonkey::work::difficulty(doc["Difficulty"].get_double().value);

        return Gigamonkey::Bitcoin::headers::header(hashDigest,header,Height,diff);
    }

    Gigamonkey::Bitcoin::headers::header DB::operator[](const digest256 &digest) const {
        initialize();
        auto hashString=data::encoding::hex::write(digest, data::endian::order::little, data::encoding::hex::letter_case::lower);
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                headers_collection.find_one(document{} << "Hash" << hashString << finalize);
        if(maybe_result) {
            auto doc=maybe_result.value();
            auto header= from_document(doc);
            return header;
        }
        else {
            return Gigamonkey::Bitcoin::headers::header();
        }
    }

    const bsoncxx::document::value DB::transaction_to_document(
            data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> transaction) {
        auto builder = bsoncxx::builder::stream::document{};
        auto txHash=data::encoding::hex::write(transaction.Key, data::endian::order::little, data::encoding::hex::letter_case::lower);
        auto hashString=data::encoding::hex::write(transaction.value().Header.hash(), data::endian::order::little, data::encoding::hex::letter_case::lower);

        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                headers_collection.find_one(document{} << "Hash" << hashString << finalize);
        if(maybe_result) {

            auto doc = maybe_result->view();
            auto tmpBytes=transaction.Value.get();
            auto data=data::encoding::hex::write(*tmpBytes, data::endian::order::little, data::encoding::hex::letter_case::lower);
            auto tmp=doc["_id"].get_oid();

            bsoncxx::document::value doc_value = builder
                    << "Hash" << txHash
                    << "Data" << data
                    << "Header" << bsoncxx::builder::stream::open_document
                    << "$ref" << "headers"
                    << "$id" << tmp
                    << "$db" << "Cosmos"
                    << bsoncxx::builder::stream::close_document
                    << bsoncxx::builder::stream::finalize;
            return doc_value;
        }
        return bsoncxx::document::value(nullptr, 0, nullptr);
    }

    data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry>
    DB::transaction_from_document(bsoncxx::document::value document) const {
        //return data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry>(Gigamonkey::digest(),
         //                                                                                        ledger::double_entry());
        auto doc=document.view();
        string data=string(doc["Data"].get_utf8().value);
        bytes trans=data::bytes_view(data::encoding::hex::view{data});
        auto ptr=std::make_shared<bytes>(trans);
        auto header=doc["Header"].get_document().view();
        auto id=header["$id"].get_oid();
        bsoncxx::document::value filter_document =  bsoncxx::builder::stream::document{}
                << "_id" << id
                << finalize;
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                headers_collection.find_one(filter_document.view());
        if(!maybe_result)
            throw "Transaction without a header";
        auto db_header=from_document(maybe_result.value());
        string txIdHash=doc["Hash"].get_utf8().value.to_string();
        auto txId=digest256 ("0x"+txIdHash);
        return data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry>(txId,Gigamonkey::Bitcoin::ledger::double_entry(ptr, Merkle::proof(), db_header.Header));
    }

    bool DB::insert(
            const data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> &transaction) {
        initialize();
        auto doc=transaction_to_document(transaction);
        try {
            bsoncxx::stdx::optional<mongocxx::result::insert_one> result = transaction_collection.insert_one(
                    std::move(doc));
            return (bool)result;
        }
        catch(std::exception e) {

        }

        return false;
    }

    data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry>
    DB::get_transaction(const digest256 &digest) {
        initialize();
        auto hashString=data::encoding::hex::write(digest, data::endian::order::little, data::encoding::hex::letter_case::lower);
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                transaction_collection.find_one(document{} << "Hash" << hashString << finalize);
        if(maybe_result) {
            auto doc=maybe_result.value();
            auto entry= transaction_from_document(doc);
            return entry;
        } else {
            return data::entry<Gigamonkey::Bitcoin::txid,Gigamonkey::Bitcoin::ledger::double_entry>(Gigamonkey::Bitcoin::txid(), Gigamonkey::Bitcoin::ledger::double_entry());
        }
    }
}

