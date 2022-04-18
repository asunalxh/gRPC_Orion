#include "RocksDBConnector.h"
#include <iostream>

using rocksdb::Options;
using rocksdb::WriteOptions;
using rocksdb::ReadOptions;
using std::cout;

RocksDBConnector::RocksDBConnector(string path) : DBConnector()
{
	Options options;
	options.create_if_missing = true;
	rocksdb::Status status = DB::Open(options, path, &db);

	if (!status.ok())
	{
		cout << "Open Database Error: " << status.ToString() << '\n';
	}
}

string RocksDBConnector::Get(string key)
{
	string value;
	this->Get(key, &value);
	return value;
}

bool RocksDBConnector::Get(const void *key, int key_len, void *value)
{
	Slice key_str((const char *)key, key_len);
	string value_str;
	bool flag = this->Get(key_str, &value_str);

	if (flag)
		memcpy(value, value_str.c_str(), value_str.length());
	return flag;
}

bool RocksDBConnector::Put(string key, string value)
{
	this->Put(Slice(key), Slice(value));
}

bool RocksDBConnector::Put(const void *key, int key_len, const void *value, int value_len)
{
	return this->Put(Slice((const char *)key, key_len), Slice((const char *)value, value_len));
}

bool RocksDBConnector::Get(Slice key, string *value)
{
	rocksdb::Status status = db->Get(ReadOptions(), key, value);
	if (!status.ok())
	{
		cout << "Get Value Error: " << status.ToString() << '\n';
		return false;
	}
	return true;
}

bool RocksDBConnector::Put(Slice key, Slice value)
{
	rocksdb::Status status = db->Put(WriteOptions(), key, value);
	if (!status.ok())
	{
		cout << "Put Value Error: " << status.ToString() << '\n';
		false;
	}
	return true;
}

RocksDBConnector::~RocksDBConnector()
{
	delete db;
}