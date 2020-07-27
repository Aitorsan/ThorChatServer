#ifndef DATABASE_ACCESS_H
#define DATABASE_ACCESS_H
extern "C"
{
    #include "sqlite3/sqlite3.h"
}
#include <string>
#include <memory>

class DataBaseAccess
{
using Sqlite3Database = std::unique_ptr <sqlite3,decltype(&sqlite3_close)>;
using SqlStatement = std::unique_ptr <sqlite3_stmt,decltype(&sqlite3_finalize)>;

public:
    
    DataBaseAccess(const char* dbname) ;
    ~DataBaseAccess();
     
    bool logInDB();
    void closeDB();

    std::string querryString(const char* querry);
    
private:
    
    Sqlite3Database openDataBase( const char* dbName);
    SqlStatement createSqlStatement(const std::string& sqlStatement);
    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();
    void execute(const char* sql);
   
    Sqlite3Database db;

};

#endif