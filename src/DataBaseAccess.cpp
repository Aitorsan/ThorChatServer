#include "DataBaseAcces.h"
#include <iostream>


DataBaseAccess::DataBaseAccess(const char *dbname)
 :db{nullptr,nullptr}
{
   //Note: this is being moved
   db = openDataBase(dbname);

}
DataBaseAccess::~DataBaseAccess()
{
}

bool DataBaseAccess::logInDB()
{
    return true;
}
void DataBaseAccess::closeDB()
{
}

std::string DataBaseAccess::querryString(const char *querry)
{
    return "found";
}


DataBaseAccess::database DataBaseAccess::openDataBase( const char* dbName)
{

	sqlite3* db = nullptr;
	auto rc = sqlite3_open(dbName,&db);

	if ( rc != SQLITE_OK)
	{
		std::cerr << "Unable to open database" << dbName << " : "<< sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		std::exit(EXIT_FAILURE);
	}

	return database{db,sqlite3_close};
}
