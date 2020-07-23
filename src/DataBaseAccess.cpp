#include "DataBaseAcces.h"

DataBaseAccess::DataBaseAccess(const char *dbname)
{
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