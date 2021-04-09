//
// Created by nekosune on 27/03/2021.
//
#include "mongodb_db.h"
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::make_array;
namespace Gigamonkey {
    namespace Bitcoin {
        headers::header::header() {

        }
    }
}
namespace Cosmos {
    bool MongoDB_DB::initialized;
    mongocxx::instance MongoDB_DB::instance{};
    mongocxx::client MongoDB_DB::client;
    mongocxx::database MongoDB_DB::db;
    mongocxx::collection MongoDB_DB::headers_collection;
    mongocxx::collection MongoDB_DB::transaction_collection;

    Gigamonkey::Bitcoin::headers::header MongoDB_DB::latest() const {
        return Gigamonkey::Bitcoin::headers::header();
    }

    Gigamonkey::Bitcoin::headers::header MongoDB_DB::operator[](const Gigamonkey::N &n) const {
        MongoDB_DB::initialize();
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                MongoDB_DB::headers_collection.find_one(document{} << "Height" << (double)n << finalize);
        if(maybe_result) {
                auto doc=maybe_result.value();
                auto header= from_document(doc);
                return header;
        }
        else {
            return Gigamonkey::Bitcoin::headers::header();
        }
    }

    Gigamonkey::Merkle::dual MongoDB_DB::dual_tree(const Gigamonkey::digest256 &digest256) const {
        return Gigamonkey::Merkle::dual();
    }

    Gigamonkey::Merkle::proof MongoDB_DB::proof(const Gigamonkey::Bitcoin::txid &txid) const {
        return Gigamonkey::Merkle::proof();
    }

    bool MongoDB_DB::insert(const header &header) {
        MongoDB_DB::initialize();
        auto doc=to_document(header);
        try {
            bsoncxx::stdx::optional<mongocxx::result::insert_one> result = MongoDB_DB::headers_collection.insert_one(
                    std::move(doc));
            return (bool)result;
        }
        catch(std::exception e) {

        }

        return false;
    }

    bool MongoDB_DB::insert(const Gigamonkey::Merkle::proof &proof) {
        return false;
    }

    void MongoDB_DB::initialize() {
        if(MongoDB_DB::initialized)
            return;

        MongoDB_DB::client=mongocxx::client(mongocxx::uri{});
        MongoDB_DB::db = client["Cosmos"];
        MongoDB_DB::headers_collection = db["headers"];
        MongoDB_DB::transaction_collection=db["transactions"];

        mongocxx::options::index index_options{};
        index_options.unique(true);
        // Header indexes
        int header_index_count=std::distance(MongoDB_DB::headers_collection.indexes().list().begin(), MongoDB_DB::headers_collection.indexes().list().end());
        if(header_index_count !=2) {
            MongoDB_DB::headers_collection.create_index(make_document(kvp("Height", 1), kvp("Hash", 1)),
                                                             index_options);
        }
        // Transaction index
        int transaction_index_count=std::distance(MongoDB_DB::transaction_collection.indexes().list().begin(), MongoDB_DB::transaction_collection.indexes().list().end());
        if(transaction_index_count == 0) {
            MongoDB_DB::transaction_collection.create_index(make_document(kvp("Hash", 1)),
                                                        index_options);
        }
        std::cout << "Transaction index: " << transaction_index_count << std::endl;

        MongoDB_DB::initialized = true;
    }

    const bsoncxx::document::value MongoDB_DB::to_document(Gigamonkey::Bitcoin::headers::header header) {
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

    Gigamonkey::Bitcoin::headers::header MongoDB_DB::from_document(bsoncxx::document::value document) const {
        Gigamonkey::Bitcoin::header header;
        auto doc=document.view();
        std::string previousString=doc["Previous"].get_utf8().value.to_string();
        header.Previous = Gigamonkey::digest256 ("0x"+previousString);
        header.Version = doc["Version"].get_int32().value;
        std::string markleString=doc["MerkleRoot"].get_utf8().value.to_string();
        header.MerkleRoot = Gigamonkey::digest256 ("0x"+markleString);
        header.Timestamp = Gigamonkey::Bitcoin::timestamp(doc["Timestamp"].get_int64().value);
        header.Target = Gigamonkey::Bitcoin::target(static_cast<uint32_t>(doc["Target"].get_int64().value));
        header.Nonce = static_cast<uint32_t>(doc["Nonce"].get_int64().value);
        std::string hashString = doc["Hash"].get_utf8().value.to_string();
        Gigamonkey::digest256 hashDigest("0x"+hashString);
        Gigamonkey::N  Height = Gigamonkey::N(doc["Height"].get_double().value);
        Gigamonkey::work::difficulty diff=Gigamonkey::work::difficulty(doc["Difficulty"].get_double().value);

        return Gigamonkey::Bitcoin::headers::header(hashDigest,header,Height,diff);
    }

    Gigamonkey::Bitcoin::headers::header MongoDB_DB::operator[](const Gigamonkey::digest256 &digest) const {
        MongoDB_DB::initialize();
        auto hashString=data::encoding::hex::write(digest, data::endian::order::little, data::encoding::hex::letter_case::lower);
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                MongoDB_DB::headers_collection.find_one(document{} << "Hash" << hashString << finalize);
        if(maybe_result) {
            auto doc=maybe_result.value();
            auto header= from_document(doc);
            return header;
        }
        else {
            return Gigamonkey::Bitcoin::headers::header();
        }
    }

    const bsoncxx::document::value MongoDB_DB::transaction_to_document(
            data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> transaction) {
        auto builder = bsoncxx::builder::stream::document{};
        auto txHash=data::encoding::hex::write(transaction.Key, data::endian::order::little, data::encoding::hex::letter_case::lower);
        auto hashString=data::encoding::hex::write(transaction.value().Header.hash(), data::endian::order::little, data::encoding::hex::letter_case::lower);

        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                MongoDB_DB::headers_collection.find_one(document{} << "Hash" << hashString << finalize);
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
    MongoDB_DB::transaction_from_document(bsoncxx::document::value document) const {
        //return data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry>(Gigamonkey::digest(),
         //                                                                                        ledger::double_entry());
        auto doc=document.view();
        std::string data=std::string(doc["Data"].get_utf8().value);
        Gigamonkey::bytes trans=data::bytes_view(data::encoding::hex::view{data});
        auto ptr=std::make_shared<Gigamonkey::bytes>(trans);
        auto header=doc["Header"].get_document().view();
        auto id=header["$id"].get_oid();
        bsoncxx::document::value filter_document =  bsoncxx::builder::stream::document{}
                << "_id" << id
                << finalize;
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                MongoDB_DB::headers_collection.find_one(filter_document.view());
        if(!maybe_result)
            throw "Transaction without a header";
        auto db_header=from_document(maybe_result.value());
        std::string txIdHash=doc["Hash"].get_utf8().value.to_string();
        auto txId=Gigamonkey::digest256 ("0x"+txIdHash);
        return data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry>(txId,Gigamonkey::Bitcoin::ledger::double_entry(ptr,Gigamonkey::Merkle::proof(),db_header.Header));
    }

    bool MongoDB_DB::insert(
            const data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry> &transaction) {
        MongoDB_DB::initialize();
        auto doc=transaction_to_document(transaction);
        try {
            bsoncxx::stdx::optional<mongocxx::result::insert_one> result = MongoDB_DB::transaction_collection.insert_one(
                    std::move(doc));
            return (bool)result;
        }
        catch(std::exception e) {

        }

        return false;
    }

    data::entry<Gigamonkey::Bitcoin::txid, Gigamonkey::Bitcoin::ledger::double_entry>
    MongoDB_DB::get_transaction(const Gigamonkey::digest256 &digest) {
        MongoDB_DB::initialize();
        auto hashString=data::encoding::hex::write(digest, data::endian::order::little, data::encoding::hex::letter_case::lower);
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                MongoDB_DB::transaction_collection.find_one(document{} << "Hash" << hashString << finalize);
        if(maybe_result) {
            auto doc=maybe_result.value();
            auto entry= transaction_from_document(doc);
            return entry;
        }
        else {
            return data::entry<Gigamonkey::Bitcoin::txid,Gigamonkey::Bitcoin::ledger::double_entry>(Gigamonkey::Bitcoin::txid(), Gigamonkey::Bitcoin::ledger::double_entry());
        }
    }
}

