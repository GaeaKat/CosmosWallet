#include <iostream>
#include <data/networking/http.hpp>
#include "include/matterpool_timechain.h"
#include "include/mongodb_db.h"

#include <mysql/jdbc.h>

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

    string url = "tcp://url-no-port";
    string user="username";
    string password="password";
    string db="dbaname";
    try {
        sql::Driver * driver = sql::mysql::get_driver_instance();
        std::cout << "Creating session on " << url << std::endl;
        std::unique_ptr<sql::Connection> con(driver->connect(url,user,password));
        con->setSchema(db);
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW TABLES"));
        std::cout << "Tables in DB: " <<std::endl;
        while(res->next()) {
            std::cout << res->getString(1) << std::endl;
        }

    }
    catch (sql::SQLException &e)
    {
        /*
          The JDBC API throws three different exceptions:

        - sql::MethodNotImplementedException (derived from sql::SQLException)
        - sql::InvalidArgumentException (derived from sql::SQLException)
        - sql::SQLException (derived from std::runtime_error)
        */

        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << "EXAMPLE_FUNCTION" << ") on line " << __LINE__ << std::endl;

        /* Use what() (derived from std::runtime_error) to fetch the error message */

        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

        return EXIT_FAILURE;
    }



    return 0;
}
