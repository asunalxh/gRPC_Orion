#include "MysqlConnector.h"
#include <iostream>
#include <cstring>
#include <string>

namespace MysqlConnector
{
	BaseConnector::BaseConnector(MYSQL *mysql, string table)
	{
		this->mysql = mysql;
		this->table = table;
	}

	void BaseConnector::UseTable(string table)
	{
		this->table = table;
	}

	bool BaseConnector::InsertValue(string id, string value)
	{
		string sqlstr = "replace into " + table + " values(\"" + id + "\",\"" + value + "\")";
		int ret = mysql_real_query(mysql, sqlstr.c_str(), sqlstr.length());
		if (ret)
		{
			printf("Mysql Insert Error: %s\n", mysql_error(mysql));
			return false;
		}
		return true;
	}

	bool BaseConnector::SearchValue(string id, string &value)
	{
		string sqlstr = "select value from " + table + " where id = \"" + id + "\"";
		MYSQL_RES *res;
		if (!Query_For_Result(sqlstr, res))
		{
			return false;
		}

		MYSQL_ROW row = mysql_fetch_row(res);
		value = string(row[0]);
		mysql_free_result(res);
		return true;
	}

	bool BaseConnector::Query_For_Result(string sqlstr, MYSQL_RES *&res)
	{

		int ret = mysql_query(mysql, sqlstr.c_str());
		if (ret)
		{
			printf("Mysql Select Error: %s\n", mysql_error(mysql));
			return false;
		}

		res = mysql_store_result(mysql);
		if (res == NULL)
		{
			printf("Mysql Store Error: %s\n", mysql_error(mysql));
			return false;
		}
		return true;
	}

	// StringMapper
	StringMapper::StringMapper(MYSQL *mysql, string table) : BaseConnector(mysql, table), DBConnector()
	{
	}

	string StringMapper::Get(string key)
	{
		string value;
		SearchValue(key, value);
		return value;
	}

	void StringMapper::Put(string key, string value)
	{
		InsertValue(key, value);
	}

	// CacheReader

	CacheReader::CacheReader(MYSQL *mysql, string table, int cache_size) : BaseConnector(mysql, table), DBConnector()
	{
		this->cache_size = cache_size;
		cache.resize(cache_size);
	}

	string CacheReader::Get(int id)
	{
		if (id <= Last_End_Id && id > Last_End_Id - cache_size)
		{
			return cache[(id - 1) % cache_size];
		}

		int start = id - (id - 1) % cache_size;
		int end = start + cache_size - 1;
		string sqlstr = "select value from " + table + " where id between " +
						std::to_string(start) + " and " + std::to_string(end);

		MYSQL_RES *res;
		if (Query_For_Result(sqlstr, res))
		{
			MYSQL_ROW row;
			int i = 0;
			while (row = mysql_fetch_row(res))
			{
				cache[i] = string(row[0]);
				i++;
			}
		}
		mysql_free_result(res);
		this->Last_End_Id = end;
		return cache[(id - 1) % cache_size];
	}

	void CacheReader::Put(int id, string value)
	{
		InsertValue(std::to_string(id), value);
	}

	// common

	void Free_Mysql_Connect(MYSQL *mysql)
	{
		mysql_close(mysql);
	}

	MYSQL *Create_Mysql_Connect(const char *host, const char *user, const char *passwd, const char *database)
	{
		MYSQL *mysql = mysql_init(NULL);
		if (!mysql)
		{
			printf("Mysql Init Error: %s\n", mysql_error(mysql));
			return nullptr;
		}

		// 连接mysql
		mysql = mysql_real_connect(mysql, host, user, passwd, database, 3306, NULL, 0);
		if (!mysql)
		{
			printf("Mysql Connector Error %s\n", mysql_error(mysql));
			return nullptr;
		}
		return mysql;
	}
}