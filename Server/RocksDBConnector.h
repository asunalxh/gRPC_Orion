#pragma once
#include <cstdio>
#include "DBConnector.h"

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using rocksdb::Slice;
using rocksdb::DB;

class RocksDBConnector : public DBConnector
{
public:
	RocksDBConnector(string path);
	~RocksDBConnector();

	string Get(string key);
	bool Get(const void *key, int key_len, void *value);

	bool Put(string key, string value);
	bool Put(const void *key, int key_len, const void *value, int value_len);

private:
	bool GetValue(Slice key, string *value);
	bool PutValue(Slice key, Slice value);
	DB *db;
};