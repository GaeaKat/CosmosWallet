#include <iostream>
#include <data/networking/http.h>



int main() {
    std::cout << "Hello, World!" << std::endl;
    auto const host = "txdb.mattercloud.io";
    auto const port = "443";
    auto const target = "/api/v1/blockheader/0000000000000000047cb2a20adeebbf9dde18f4928e486a1b175dbac644039b?limit=1&order=asc|desc&pretty";
    data::networking::Http http;
    std::cout << http.GET(host,target) << std::endl;
    return 0;
}
