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
using database = std::unique_ptr <sqlite3,decltype(&sqlite3_close)>;

public:
    
    DataBaseAccess(const char* dbname) ;
    ~DataBaseAccess();
     
    bool logInDB();
    void closeDB();

    std::string querryString(const char* querry);
    
private:
    
    database openDataBase( const char* dbName);
    
    database db;

};

#endif