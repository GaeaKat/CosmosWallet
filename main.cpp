#include <iostream>
#include <data/networking/http.h>
#include "include/matterpool_timechain.h"
#include "include/mongodb_db.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    Cosmos::MatterPool_TimeChain timechain;
    Gigamonkey::digest<32> digest("0x9bebc37a1bd107c3ade78793a062312041e91d377b62548e9124d4e1b8df2a1b");

    auto tmp=timechain.transaction(digest);
    auto tmp2=timechain.headers(680789);
    std::cout << tmp.Value.Header << tmp << std::endl;
    auto split=timechain.header(Gigamonkey::digest<32>("0x0000000000000000011865af4122fe3b144e2cbeea86142e8ff2fb4107352d43"));
    std::cout << "Timestamp of bsv spliut:"  << (long)split.Header.Timestamp << std::endl;
    std::cout <<"Price: " << timechain.price(tmp.Value.Header.Timestamp) <<std::endl;
    Gigamonkey::Bitcoin::address addr("18FnwHbZz5wwCxJ4h2sQsAMYd7qyHryJUX");
    auto transactions=timechain.transactions(addr);
    for(const auto& tmpTrans:transactions) {
        std::cout  << tmpTrans.Value.Header << std::endl;
    }
    return 0;
}
