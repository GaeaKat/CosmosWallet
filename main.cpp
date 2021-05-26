#include <iostream>
#include <data/networking/http.hpp>
#include "include/matterpool_timechain.h"
#include "include/mongodb_db.h"
#include "include/sql.h"

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

    try {
        pqxx::connection C(
                "dbname = dbname user = user password = password hostaddr =address port = 5432");
        if (C.is_open()) {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        } else {
            std::cout << "Can't open database" << std::endl;
            return 1;
        }
        //* Create SQL statement *//
        char *sql = "select * from \"boost_job\"";

        pqxx::nontransaction N(C);

        pqxx::result R(N.exec(sql));
        std::cout << "Postgres tables" << std::endl;
        int count=0;
        std::vector<std::string> invalids;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
            auto txid=c[0].as<std::string>();
            bool spent=!c[6].is_null();
            if(!spent) {
                auto check=timechain.transaction(Gigamonkey::digest<32>("0x"+txid));
                if(!check.valid()) {
                    count++;
                    invalids.push_back(txid);
                }
            }
        }
        std::cout << "Count: " << count;
        N.commit();
        pqxx::work w(C);
        for(const auto& txid: invalids) {
            w.exec("DELETE FROM \"boost_job\" WHERE txid = '"+txid+"'");
        }
        w.commit();

        C.disconnect();
    }catch (const std::exception &e) {
        std::cerr <<"Errored: " << e.what()  << std::endl;
        return 1;
    }



    return 0;
}
