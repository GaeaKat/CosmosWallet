
#include "sql.h"

namespace Cosmos {
    
    list<string> sql_session::show_tables() {
        
        std::unique_ptr< sql::ResultSet > result = execute("SHOW TABLES");
        
        list<string> tables;
        while(result->next()) {
            tables = tables << result->getString(1);
        }
        
        return tables;
        
    }

}
