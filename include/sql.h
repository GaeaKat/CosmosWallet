// Copyright (c) 2021 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOSWALLET_SQL
#define COSMOSWALLET_SQL

#include "types.h"
#include <mysql/jdbc.h>

namespace Cosmos {
    
    struct sql_session final {
    
        struct parameters {
            string URL;
            string User;
            string Password;
            string DB;
        };
        
        std::unique_ptr<sql::ResultSet> execute(const string &query) {
            return std::unique_ptr<sql::ResultSet>{Connection->createStatement()->executeQuery(query)};
        }
        
        static sql_session connect_mysql(const parameters &p) {
            sql_session x{sql::mysql::get_driver_instance()->connect(p.URL, p.User, p.Password)};
            x.Connection->setSchema(p.DB);
            return x;
        }
        
        // TODO
        static sql_session postgres_connect(const parameters &p);
        
        ~sql_session() {
            delete Connection;
        }
        
    private:
        sql::Connection *Connection;
        sql_session(sql::Connection *conn) : Connection{conn} {}
    };
    
    
}

#endif
