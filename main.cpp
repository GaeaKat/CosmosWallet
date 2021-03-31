#include <iostream>
#include <data/networking/http.h>
#include "include/matterpool_timechain.h"
#include "include/mongodb_headers.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    Cosmos::MatterPool_TimeChain timechain;
    Gigamonkey::digest<32> digest("0x9bebc37a1bd107c3ade78793a062312041e91d377b62548e9124d4e1b8df2a1b");

    auto tmp=timechain.transaction(digest);

    std::cout << tmp.Value.Header << tmp << std::endl;
    Cosmos::MatterpoolApi api;
    std::cout <<"Price: " << api.price(tmp.Value.Header.Timestamp) <<std::endl;
    return 0;
}
