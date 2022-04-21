#include "MysqlConnector.h"
#include <iostream>
#include <cstring>
using std::cout;
using std::endl;

MysqlConnector::MysqlConnector(const char *host, const char *user, const char *passwd, const char *database)
{
	mysql = mysql_init(mysql);
	if (!mysql)
	{
		printf("Mysql Init Error: %s\n", mysql_error(mysql));
		return;
	}

	// 连接mysql
	mysql = mysql_real_connect(mysql, host, user, passwd, database, 3306, nullptr, 0);
	if (!mysql)
	{
		printf("Mysql Connector Error %s\n", mysql_error(mysql));
	}
}

void MysqlConnector::ChangeTable(string table)
{
	this->table = table;
}

string MysqlConnector::Get(string key)
{
	string value;
	GetValue(key, value, table);
	return value;
}
bool MysqlConnector::Get(const void *key, int key_len, void *value)
{
	string result;
	bool flag = GetValue(string((const char *)key, key_len), result, table);
	if (!flag)
		return false;

	memcpy(value, result.c_str(), result.length());
	return true;
}

bool MysqlConnector::Put(string key, string value)
{
	return InsertValue(key, value, table);
}
bool MysqlConnector::Put(const void *key, int key_len, const void *value, int value_len)
{
	return InsertValue(string((const char *)key, key_len), string((const char *)value, value_len), table);
}

bool MysqlConnector::InsertValue(string id, string value, string table)
{
	string sqlstr = "replace into " + table + "value(\"" + id + "\",\"" + value + "\")";
	int ret = mysql_query(mysql, sqlstr.c_str());
	if (ret)
	{
		printf("Mysql Insert Error: %s\n", mysql_error(mysql));
	}
}

bool MysqlConnector::GetValue(string id, string &value, string table)
{
	string sqlstr = "select value from " + table + " where id = \"" + id + "\"";
	int ret = mysql_query(mysql, sqlstr.c_str());
	if (ret)
	{
		printf("Mysql Select Error: %s\n", mysql_error(mysql));
		return false;
	}

	MYSQL_RES *result = mysql_store_result(mysql);
	if (result == NULL)
	{
		printf("Mysql Store Error: %s\n", mysql_error(mysql));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(result);
	value = row[0];
	return true;
}

MysqlConnector::~MysqlConnector()
{
	mysql_close(mysql);
}