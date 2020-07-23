#ifndef DATABASE_ACCESS_H
#define DATABASE_ACCESS_H
#include <string>
class DataBaseAccess
{
public:
    
    DataBaseAccess(const char* dbname) ;
    ~DataBaseAccess();
     
    bool logInDB();
    void closeDB();

    std::string querryString(const char* querry);
    
};

#endif