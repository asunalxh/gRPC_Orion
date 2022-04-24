#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <string>
using std::string;

class DBConnector
{
public:
	virtual string Get(string key) = 0;
	virtual bool Get(const void *key, int key_len, void *value) = 0;

	virtual bool Put(string key, string value) = 0;
	virtual bool Put(const void *key, int key_len, const void *value, int value_len) = 0;
};

#endif