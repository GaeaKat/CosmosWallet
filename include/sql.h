// Copyright (c) 2021 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOSWALLET_SQL
#define COSMOSWALLET_SQL

#include "types.h"
#include <mysql/jdbc.h>
#include <pqxx/pqxx>

namespace Cosmos::SQL {
    
    enum type {
        MYSQL,
        POSTGRES
    };
    
    struct parameters {
        string URL;
        string User;
        string Password;
        string DB;
    };
    
    struct description {
        
        string Name;
        string Type;
        string Null;
        string Key;
        string Default;
        string Extra;
        
    };
    
    struct session {
        
        virtual list<string> show_tables() = 0;
        virtual list<description> describe(const string& table) = 0;
        virtual int64_t rows_in_table(const string& table) = 0;
        
        virtual ~session() {}
        
    };
    
    struct mysql final : session {
        
        std::unique_ptr<sql::ResultSet> execute(const string &query) {
            return std::unique_ptr<sql::ResultSet>{Connection->createStatement()->executeQuery(query)};
        }
        
        static mysql *connect(const parameters &p) {
            mysql* x = new mysql{sql::mysql::get_driver_instance()->connect(p.URL, p.User, p.Password)};
            x->Connection->setSchema(p.DB);
            return x;
        }
        
        ~mysql() {
            delete Connection;
        }
        
        list<string> show_tables() override;
        list<description> describe(const string& table) override;
        int64_t rows_in_table(const string& table) override;
        
    private:
        sql::Connection *Connection;
        mysql(sql::Connection *conn) : Connection{conn} {}
    };
    
    struct postgres final : session {
        
        pqxx::result execute(const string &query) {
            return pqxx::nontransaction(Connection).exec(query);
        }
        
        list<string> show_tables() override;
        
        list<description> describe(const string& table) override;
        int64_t rows_in_table(const string& table) override;
        
        static postgres *connect(const parameters &p) {
            return new postgres{p};
        }
        
        ~postgres() {
            Connection.disconnect();
        }
        
    private:
        pqxx::connection Connection;
        string DBName;
        postgres(const parameters &p) : Connection{
            string{} + "dbname=" + p.DB + " user=" + p.User + " password=" + p.Password + " hostaddr=" + p.URL + " port = 5432"}, DBName{p.DB} {}
    };
    
    std::ostream inline &operator<<(std::ostream &o, const description& d) {
        return o << "[\"" 
            << d.Name << "\", \"" 
            << d.Type << "\", \"" 
            << d.Null << "\", \"" 
            << d.Key << "\", \"" 
            << d.Default << /*"\", \"" 
            << d.Extra <<*/ "\"]";
    }
    
}

#endif
