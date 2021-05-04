
#include "sql.h"

namespace Cosmos::SQL {
    
    list<string> mysql::show_tables() {
        
        std::unique_ptr< sql::ResultSet > result = execute("SHOW TABLES");
        
        list<string> tables;
        while(result->next()) {
            tables = tables << result->getString(1);
        }
        
        return tables;
        
    }
    
    list<description> mysql::describe(const string& table) {
        
        string query = string{"DESCRIBE "} + table;
        
        std::unique_ptr< sql::ResultSet > result = execute(query);
        
        list<description> rows;
        
        while(result->next()) {
            rows = rows << description{
                result->getString(1), 
                result->getString(2), 
                result->getString(3), 
                result->getString(4), 
                result->getString(5), 
                result->getString(6)};
        } 
        
        return rows;
        
    }
        
    list<string> postgres::show_tables() {
        pqxx::result result(pqxx::nontransaction(Connection).exec("SELECT * FROM pg_catalog.pg_tables WHERE schemaname != 'pg_catalog' AND schemaname != 'information_schema'"));
        
        list<string> tables;
        
        for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
            tables = tables << c[1].as<std::string>();
        }
        
        return tables;
    }
        
    list<description> postgres::describe(const string& table) {
        string query = string{"select column_name, data_type, character_maximum_length, column_default, is_nullable from INFORMATION_SCHEMA.COLUMNS where table_name = \'"} + table + "\'";
        
        pqxx::result result(pqxx::nontransaction(Connection).exec(query));
        
        list<description> rows;
        
        for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
            
            rows = rows << description{
                c[0].is_null() ? string{} : c[0].as<string>(), 
                c[1].is_null() ? string{} : c[1].as<string>(), 
                c[2].is_null() ? string{} : c[2].as<string>(), 
                c[3].is_null() ? string{} : c[3].as<string>(), 
                c[4].is_null() ? string{} : c[4].as<string>(), 
                c[5].is_null() ? string{} : c[5].as<string>()};
        }
        
        return rows;
    }
    
    int64_t mysql::rows_in_table(const string& table) {
        string query = string{"SELECT COUNT(*) FROM "} + table;
        
        std::unique_ptr< sql::ResultSet > result = execute(query);
        
        list<description> rows;
        
        if(result->next()) return result->getInt(1);
            
        return -1;
        
    }
    
    int64_t postgres::rows_in_table(const string& table) {
        string query = string{"SELECT COUNT(*) FROM "} + table;
        
        // string query = string{"select sum([rows]) from sys.partitions where object_id=object_id('"} + table + "') and index_id in (0,1)";
        pqxx::result result(pqxx::nontransaction(Connection).exec(query));
        
        return result.begin()[1].as<int64_t>();
    }

}
