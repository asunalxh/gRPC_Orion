#ifndef MYSQLCONNECTOR_H
#define MYSQLCONNECTOR_H

#include <mysql.h>
#include <string>
#include "DBConnector.h"
#include <vector>

using std::string;
using std::vector;

namespace MysqlConnector
{
	class BaseConnector
	{
	public:
		BaseConnector(MYSQL *mysql, string table);
		void UseTable(const string &table);

	protected:
		bool InsertValue(const string &id, const string &value);
		bool SearchValue(const string &id, string &value);
		bool Query_For_Result(const string &sqlstr, MYSQL_RES *&res);
		MYSQL *mysql;
		string table;
	};

	class StringMapper : public BaseConnector, public DBConnector<string, string>
	{
	public:
		StringMapper(MYSQL *mysql, const string &table);
		bool Get(const string &id, string &value);
		bool Put(const string &id, const string &value);
		bool Delete(const string &id);
	};

	class CacheReader : public BaseConnector, public DBConnector<int, string>
	{
	public:
		CacheReader(MYSQL *mysql, const string &table, int cache_size);
		bool Get(const int &id, string &value);
		bool Put(const int &id, const string &value);
		bool Delete(const int &id);

	private:
		int cache_size;
		int Last_End_Id = 0;
		vector<string> cache;
	};

	void Free_Mysql_Connect(MYSQL *mysql);
	MYSQL *Create_Mysql_Connect(const char *host, const char *user, const char *passwd, const char *database);
}

#endif
