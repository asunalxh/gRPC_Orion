#include <mysql.h>
#include <string>
#include "DBConnector.h"

using std::string;
class MysqlConnector final : public DBConnector
{
public:
	MysqlConnector(const char *host, const char *user, const char *passwd, const char *database);
	~MysqlConnector();

	void UseTable(string table);

	string Get(string key);
	bool Get(const void *key, int key_len, void *value);

	bool Put(string key, string value);
	bool Put(const void *key, int key_len, const void *value, int value_len);

private:
	bool InsertValue(string id, string value, string table);
	bool GetValue(string id, string &value, string table);
	MYSQL *mysql;
	string table;
};