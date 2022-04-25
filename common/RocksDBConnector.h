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
		BaseConnector(string path);
		~BaseConnector();

	protected:
		bool GetValue(Slice key, string *value);
		bool PutValue(Slice key, Slice value);
		DB *db;
	};

	class StringMapper : public BaseConnector, public DBConnector<string, string>
	{
	public:
		StringMapper(string path);
		string Get(string key);
		void Put(string key, string value);
	};

	class IntMapper : public BaseConnector, public DBConnector<int, string>
	{
	public:
		IntMapper(string path);
		string Get(int key);
		void Put(int key, string value);
	};
}

#endif
