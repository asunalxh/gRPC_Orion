#ifndef MYSQLCONNECTOR_H
#define MYSQLCONNECTOR_H

#include <mysql.h>
#include <string>
#include "DBConnector.h"

using std::string;
class MysqlConnector final : public DBConnector
{
public:
	MysqlConnector(MYSQL* mysql, string table);

	void UseTable(string table);

	string Get(string key);
	bool Get(const void *key, int key_len, void *value);

	bool Put(string key, string value);
	bool Put(const void *key, int key_len, const void *value, int value_len);


	static MYSQL* Free_Mysql_Connect(MYSQL* mysql);
	static MYSQL* Create_Mysql_Connect(const char* host,const char* user,const char* passwd,const char* database);


private:
	bool InsertValue(string id, string value, string table);
	bool GetValue(string id, string &value, string table);
	MYSQL *mysql;
	string table;
};

#endif