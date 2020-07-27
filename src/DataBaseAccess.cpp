#include "DataBaseAcces.h"
#include <iostream>

DataBaseAccess::DataBaseAccess(const char *dbname)
	: db{nullptr, nullptr}
{
	//Note: this is being moved
	db = openDataBase(dbname);
}
DataBaseAccess::~DataBaseAccess()
{
	closeDB();
}

bool DataBaseAccess::logInDB()
{
	return true;
}
void DataBaseAccess::closeDB()
{
	
}

// compile sql statmentes or queries so the database does not habe to compile them 
DataBaseAccess::SqlStatement DataBaseAccess::createSqlStatement(const std::string& sqlStatement)
{
	sqlite3_stmt* stmt = nullptr;

	int rc = sqlite3_prepare_v2 ( db.get(),sqlStatement.c_str(),sqlStatement.length(),&stmt,nullptr);
	
	if (rc != SQLITE_OK)
	{
		std::cerr << "Unable to compile statement" << sqlStatement << " : " << sqlite3_errmsg(db.get()) << '\n';
		std::exit(EXIT_FAILURE);
	}
	
	return SqlStatement{stmt,sqlite3_finalize};
}

std::string DataBaseAccess::querryString(const char *querry)
{
	beginTransaction();
	execute(querry);
	commitTransaction();
	rollbackTransaction();
}

DataBaseAccess::Sqlite3Database DataBaseAccess::openDataBase(const char *dbName)
{
	sqlite3 *db = nullptr;
	auto rc = sqlite3_open(dbName, &db);

	if (rc != SQLITE_OK)
	{
		std::cerr << "Unable to open database" << dbName << " : " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		std::exit(EXIT_FAILURE);
	}

	return Sqlite3Database{db, sqlite3_close};
}
  void DataBaseAccess::beginTransaction()
  {
	  execute("BEGIN TRANSACTION");
  }
   
void DataBaseAccess::commitTransaction()
{
	execute("COMMIT TRANSACTION");
}
void DataBaseAccess::rollbackTransaction()
{
	execute("ROLLBACK TRANSACTION");
}

void DataBaseAccess::execute(const char *sql)
{
	if (db)
	{
		char *errmsg = 0;
		int rc = sqlite3_exec(db.get(), sql, nullptr, 0, &errmsg);
		if (rc != SQLITE_OK)
		{
			std::cerr << "Unable to execute " << sql << ": " << errmsg << '\n';
			sqlite3_free(errmsg);
			std::exit(EXIT_FAILURE);
		}
	}
	else
	{
		std::cerr << "NO database conexion object available\n";
		std::exit(EXIT_FAILURE);
	}
}
