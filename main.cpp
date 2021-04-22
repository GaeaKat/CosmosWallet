#include <iostream>
#include <data/networking/http.hpp>
#include "include/matterpool_timechain.h"
#include "include/mongodb_db.h"

int main() {
    using namespace Cosmos;
    std::cout << "Hello, World!" << std::endl;
    ptr<data::networking::http> http = std::make_shared<data::networking::http>();
    Cosmos::MatterPool::TimeChain timechain(http);
    Gigamonkey::digest<32> digest("0x874999FF562C52EFF5062514FF1EDD1516741C1B7A0919B49B56E427071E8081");

    auto tmp=timechain.transaction(digest);
    auto tmp2=timechain.transaction(digest);
    std::cout << tmp.Value.Header << " tmp " << tmp << std::endl;
    std::cout << tmp2.Value.Header << " tmp2 " << tmp2 << std::endl;
    return 0;
}
