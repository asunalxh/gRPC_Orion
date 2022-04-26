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
		BaseConnector(string path);
		~BaseConnector();

	protected:
		bool SearchValue(Slice key, string *value);
		bool InsertValue(Slice key, Slice value);
		bool DeleteValue(Slice Key);
		DB *db;
	};

	class StringMapper : public BaseConnector, public DBConnector<string, string>
	{
	public:
		StringMapper(string path);
		bool Get(string key, string &value);
		bool Put(string key, string value);
		bool Delete(string key);
	};

	class IntMapper : public BaseConnector, public DBConnector<int, string>
	{
	public:
		IntMapper(string path);
		bool Get(int key, string &value);
		bool Put(int key, string value);
		bool Delete(int key);
	};

	class IntStorage : public BaseConnector, public DBMap<string, int>
	{
	public:
		IntStorage(string path);
		bool Get(string key, int &value);
		bool Put(string key, int value);
		bool Delete(string key);
	};
}

#endif
