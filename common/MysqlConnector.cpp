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

	void BaseConnector::UseTable(const string &table)
	{
		this->table = table;
	}

	bool BaseConnector::InsertValue(const string &id, const string &value)
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

	bool BaseConnector::SearchValue(const string &id, string &value)
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

	bool BaseConnector::Query_For_Result(const string &sqlstr, MYSQL_RES *&res)
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
	StringMapper::StringMapper(MYSQL *mysql, const string &table) : BaseConnector(mysql, table), DBConnector()
	{
	}

	bool StringMapper::Get(const string &key, string &value)
	{
		return SearchValue(key, value);
	}

	bool StringMapper::Put(const string &key, const string &value)
	{
		return InsertValue(key, value);
	}

	bool StringMapper::Delete(const string &id)
	{
		return true;
	}

	// CacheReader

	CacheReader::CacheReader(MYSQL *mysql, const string &table, int cache_size) : BaseConnector(mysql, table), DBConnector()
	{
		this->cache_size = cache_size;
		cache.resize(cache_size);
	}

	bool CacheReader::Get(const int &id, string &value)
	{
		if (id <= Last_End_Id && id > Last_End_Id - cache_size)
		{
			value = cache[(id - 1) % cache_size];
			return true;
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
			mysql_free_result(res);
			this->Last_End_Id = end;

			if (i > (id - 1) % cache_size)
			{
				value = cache[(id - 1) % cache_size];
				return true;
			}
		}

		return false;
	}

	bool CacheReader::Put(const int &id, const string &value)
	{
		return InsertValue(std::to_string(id), value);
	}

	bool CacheReader::Delete(const int &id)
	{
		return true;
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