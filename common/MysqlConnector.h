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
		BaseConnector(MYSQL *mysql, const char *table, const char *id_col_name, const char *val_col_name);

	protected:
		bool InsertValue(const string &id, const string &value);
		bool SearchValue(const string &id, string &value);
		bool Query_For_Result(const char *sqlstr, MYSQL_RES *&res);
		MYSQL *mysql;
		const char *table;
		const char *id_col_name = "id";
		const char *val_col_name = "value";
	};

	class StringMapper : public BaseConnector, public DBConnector<string, string>
	{
	public:
		StringMapper(MYSQL *mysql, const char *table, const char *id_col_name, const char *val_col_name);
		bool Get(const string &id, string &value);
		bool Put(const string &id, const string &value);
		bool Delete(const string &id);
	};

	class IntMapper : public BaseConnector, public DBConnector<int, string>
	{
	public:
		IntMapper(MYSQL *mysql, const char *table, const char *id_col_name, const char *val_col_name);
		bool Get(const int &id, string &value);
		bool Put(const int &id, const string &value);
		bool Delete(const int &id);
	};

	class CacheReader : public IntMapper
	{
	public:
		CacheReader(MYSQL *mysql, int cache_size, const char *table, const char *id_col_name, const char *val_col_name);
		bool Get(const int &id, string &value);

	private:
		int cache_size;
		int Last_End_Id = 0;
		vector<string> cache;
	};

	void Free_Mysql_Connect(MYSQL *mysql);
	MYSQL *Create_Mysql_Connect(const char *host, const char *user, const char *passwd, const char *database);
}

#endif
