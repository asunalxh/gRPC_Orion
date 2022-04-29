#ifndef ROCKSSDBCONNECTOR_H
#define ROCKSSDBCONNECTOR_H

#include <cstdio>
#include "DBConnector.h"

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using rocksdb::DB;
using rocksdb::Slice;

namespace RocksDBConnector
{
	class BaseConnector
	{
	public:
		BaseConnector(DB *db);
		BaseConnector(const char *path);
		~BaseConnector();

	protected:
		bool SearchValue(const Slice &key, string *value);
		bool InsertValue(const Slice &key, const Slice &value);
		bool DeleteValue(const Slice &Key);
		DB *db;
	};

	class StringMapper : public BaseConnector, public DBConnector<string, string>
	{
	public:
		StringMapper(const char *path);
		bool Get(const string &key, string &value);
		bool Put(const string &key, const string &value);
		bool Delete(const string &key);
	};

	class IntMapper : public BaseConnector, public DBConnector<int, string>
	{
	public:
		IntMapper(const char *path);
		bool Get(const int &key, string &value);
		bool Put(const int &key, const string &value);
		bool Delete(const int &key);
	};

	class IntStorage : public BaseConnector, public DBMap<string, int>
	{
	public:
		IntStorage(const char *path);
		bool Get(const string &key, int &value);
		bool Put(const string &key, const int &value);
		bool Delete(const string &key);
	};
}

#endif
