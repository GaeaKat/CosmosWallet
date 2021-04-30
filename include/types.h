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


template <unsigned int BITS>
base_uint<BITS>::base_uint(const std::string &str) {
    SetHex(str);
}

template <unsigned int BITS>
base_uint<BITS> &base_uint<BITS>::operator<<=(unsigned int shift) {
    base_uint<BITS> a(*this);
    for (int i = 0; i < WIDTH; i++)
        pn[i] = 0;
    int k = shift / 32;
    shift = shift % 32;
    for (int i = 0; i < WIDTH; i++) {
        if (i + k + 1 < WIDTH && shift != 0)
            pn[i + k + 1] |= (a.pn[i] >> (32 - shift));
        if (i + k < WIDTH) pn[i + k] |= (a.pn[i] << shift);
    }
    return *this;
}

template <unsigned int BITS>
base_uint<BITS> &base_uint<BITS>::operator>>=(unsigned int shift) {
    base_uint<BITS> a(*this);
    for (int i = 0; i < WIDTH; i++)
        pn[i] = 0;
    int k = shift / 32;
    shift = shift % 32;
    for (int i = 0; i < WIDTH; i++) {
        if (i - k - 1 >= 0 && shift != 0)
            pn[i - k - 1] |= (a.pn[i] << (32 - shift));
        if (i - k >= 0) pn[i - k] |= (a.pn[i] >> shift);
    }
    return *this;
}

template <unsigned int BITS>
base_uint<BITS> &base_uint<BITS>::operator*=(uint32_t b32) {
    uint64_t carry = 0;
    for (int i = 0; i < WIDTH; i++) {
        uint64_t n = carry + (uint64_t)b32 * pn[i];
        pn[i] = n & 0xffffffff;
        carry = n >> 32;
    }
    return *this;
}

template <unsigned int BITS>
base_uint<BITS> &base_uint<BITS>::operator*=(const base_uint &b) {
    base_uint<BITS> a = *this;
    *this = 0;
    for (int j = 0; j < WIDTH; j++) {
        uint64_t carry = 0;
        for (int i = 0; i + j < WIDTH; i++) {
            uint64_t n = carry + pn[i + j] + (uint64_t)a.pn[j] * b.pn[i];
            pn[i + j] = n & 0xffffffff;
            carry = n >> 32;
        }
    }
    return *this;
}

template <unsigned int BITS>
base_uint<BITS> &base_uint<BITS>::operator/=(const base_uint &b) {
    // make a copy, so we can shift.
    base_uint<BITS> div = b;
    // make a copy, so we can subtract.
    base_uint<BITS> num = *this;
    // the quotient.
    *this = 0;
    int num_bits = num.bits();
    int div_bits = div.bits();
    if (div_bits == 0) throw uint_error("Division by zero");
    // the result is certainly 0.
    if (div_bits > num_bits) return *this;
    int shift = num_bits - div_bits;
    // shift so that div and num align.
    div <<= shift;
    while (shift >= 0) {
        if (num >= div) {
            num -= div;
            // set a bit of the result.
            pn[shift / 32] |= (1 << (shift & 31));
        }
        // shift back.
        div >>= 1;
        shift--;
    }
    // num now contains the remainder of the division.
    return *this;
}

template <unsigned int BITS>
int base_uint<BITS>::CompareTo(const base_uint<BITS> &b) const {
    for (int i = WIDTH - 1; i >= 0; i--) {
        if (pn[i] < b.pn[i]) return -1;
        if (pn[i] > b.pn[i]) return 1;
    }
    return 0;
}

template <unsigned int BITS> bool base_uint<BITS>::EqualTo(uint64_t b) const {
    for (int i = WIDTH - 1; i >= 2; i--) {
        if (pn[i]) return false;
    }
    if (pn[1] != (b >> 32)) return false;
    if (pn[0] != (b & 0xfffffffful)) return false;
    return true;
}

template <unsigned int BITS> double base_uint<BITS>::getdouble() const {
    double ret = 0.0;
    double fact = 1.0;
    for (int i = 0; i < WIDTH; i++) {
        ret += fact * pn[i];
        fact *= 4294967296.0;
    }
    return ret;
}

template <unsigned int BITS> std::string base_uint<BITS>::GetHex() const {
    return ArithToUint256(*this).GetHex();
}

template <unsigned int BITS> void base_uint<BITS>::SetHex(const char *psz) {
    *this = UintToArith256(uint256S(psz));
}

template <unsigned int BITS>
void base_uint<BITS>::SetHex(const std::string &str) {
    SetHex(str.c_str());
}

template <unsigned int BITS> std::string base_uint<BITS>::ToString() const {
    return (GetHex());
}

template <unsigned int BITS> unsigned int base_uint<BITS>::bits() const {
    for (int pos = WIDTH - 1; pos >= 0; pos--) {
        if (pn[pos]) {
            for (int bits = 31; bits > 0; bits--) {
                if (pn[pos] & 1 << bits) return 32 * pos + bits + 1;
            }
            return 32 * pos + 1;
        }
    }
    return 0;
}

#endif //COSMOSWALLET_TYPES_H
