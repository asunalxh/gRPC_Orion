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
		void UseTable(string table);

	protected:
		bool InsertValue(string id, string value);
		bool SearchValue(string id, string &value);
		bool Query_For_Result(string sqlstr, MYSQL_RES *&res);
		MYSQL *mysql;
		string table;
	};

	class StringMapper : public BaseConnector, public DBConnector<string, string>
	{
	public:
		StringMapper(MYSQL *mysql, string table);
		string Get(string id);
		void Put(string id, string value);
	};

	class CacheReader : public BaseConnector, public DBConnector<string, string>
	{
	public:
		CacheReader(MYSQL *mysql, string table, int cache_size);
		string Get(int id);
		void Put(int id, string value);

	private:
		int cache_size;
		int Last_End_Id = 0;
		vector<string> cache;
	};

	void Free_Mysql_Connect(MYSQL *mysql);
	MYSQL *Create_Mysql_Connect(const char *host, const char *user, const char *passwd, const char *database);
}

#endif
