//
// Created by nekosune on 27/03/2021.
//
#include "mongodb_headers.h"
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
    bool MongoDB_Headers::initialized;
    mongocxx::instance MongoDB_Headers::instance{};
    mongocxx::client MongoDB_Headers::client;
    mongocxx::database MongoDB_Headers::db;
    mongocxx::collection MongoDB_Headers::headers_collection;

    Gigamonkey::Bitcoin::headers::header MongoDB_Headers::latest() const {
        return Gigamonkey::Bitcoin::headers::header();
    }

    Gigamonkey::Bitcoin::headers::header MongoDB_Headers::operator[](const Gigamonkey::N &n) const {
        MongoDB_Headers::initialize();
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                MongoDB_Headers::headers_collection.find_one(document{} << "Height" << (double)n << finalize);
        if(maybe_result) {
                auto doc=maybe_result.value();
                auto header= from_document(doc);
                return header;
        }
        else {
            return Gigamonkey::Bitcoin::headers::header();
        }
    }

    Gigamonkey::Merkle::dual MongoDB_Headers::dual_tree(const Gigamonkey::digest256 &digest256) const {
        return Gigamonkey::Merkle::dual();
    }

    Gigamonkey::Merkle::proof MongoDB_Headers::proof(const Gigamonkey::Bitcoin::txid &txid) const {
        return Gigamonkey::Merkle::proof();
    }

    bool MongoDB_Headers::insert(const header &header) {
        MongoDB_Headers::initialize();
        auto doc=to_document(header);
        bsoncxx::stdx::optional<mongocxx::result::insert_one> result =MongoDB_Headers::headers_collection.insert_one(
                std::move(doc));
        return (bool)result;
    }

    bool MongoDB_Headers::insert(const Gigamonkey::Merkle::proof &proof) {
        return false;
    }

    void MongoDB_Headers::initialize() {
        if(MongoDB_Headers::initialized)
            return;

        MongoDB_Headers::client=mongocxx::client(mongocxx::uri{});
        MongoDB_Headers::db = client["Cosmos"];
        MongoDB_Headers::headers_collection = db["headers"];

        mongocxx::options::index index_options{};
        index_options.unique(true);
        int index_count=std::distance(MongoDB_Headers::headers_collection.indexes().list().begin(),MongoDB_Headers::headers_collection.indexes().list().end());
        std::cout << "index count " << index_count << std::endl;
        if(index_count>0) {
            MongoDB_Headers::headers_collection.create_index(make_document(kvp("Height", 1), kvp("Hash", 1)),
                                                             index_options);
        }
        MongoDB_Headers::initialized = true;
    }

    const bsoncxx::document::value MongoDB_Headers::to_document(Gigamonkey::Bitcoin::headers::header header) {
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

    Gigamonkey::Bitcoin::headers::header MongoDB_Headers::from_document(bsoncxx::document::value document) const {
        Gigamonkey::Bitcoin::header header;
        auto doc=document.view();
        std::string previousString=doc["Previous"].get_utf8().value.to_string();
        header.Previous = Gigamonkey::digest256 ("0x"+previousString);
        header.Version = doc["Version"].get_int32().value;
        std::cout << header.Version << std::endl;
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

    Gigamonkey::Bitcoin::headers::header MongoDB_Headers::operator[](const Gigamonkey::digest256 &digest) const {
        MongoDB_Headers::initialize();
        auto hashString=data::encoding::hex::write(digest, data::endian::order::little, data::encoding::hex::letter_case::lower);
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                MongoDB_Headers::headers_collection.find_one(document{} << "Hash" << hashString << finalize);
        if(maybe_result) {
            auto doc=maybe_result.value();
            auto header= from_document(doc);
            return header;
        }
        else {
            return Gigamonkey::Bitcoin::headers::header();
        }
    }
}

