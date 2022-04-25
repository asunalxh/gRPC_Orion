#include "RocksDBConnector.h"
#include <iostream>

using rocksdb::Options;
using rocksdb::ReadOptions;
using rocksdb::WriteOptions;
using std::cout;

namespace RocksDBConnector
{

	BaseConnector::BaseConnector(string path)
	{
		Options options;
		options.create_if_missing = true;
		rocksdb::Status status = DB::Open(options, path, &db);

		if (!status.ok())
		{
			cout << "Open Database Error: " << status.ToString() << '\n';
		}
	}

	bool BaseConnector::GetValue(Slice key, string *value)
	{
		rocksdb::Status status = db->Get(ReadOptions(), key, value);
		if (!status.ok())
		{
			cout << "Get Value Error: " << status.ToString() << '\n';
			return false;
		}
		return true;
	}

	bool BaseConnector::PutValue(Slice key, Slice value)
	{
		rocksdb::Status status = db->Put(WriteOptions(), key, value);
		if (!status.ok())
		{
			cout << "Put Value Error: " << status.ToString() << '\n';
			false;
		}
		return true;
	}

	BaseConnector::~BaseConnector()
	{
		delete db;
	}

	// StringMapper

	StringMapper::StringMapper(string path) : BaseConnector(path), DBConnector()
	{
	}

	string StringMapper::Get(string key)
	{
		string value;
		this->GetValue(key, &value);
		return value;
	}

	void StringMapper::Put(string key, string value)
	{
		this->PutValue(Slice(key), Slice(value));
	}

	// IntMapper

	IntMapper::IntMapper(string path) : BaseConnector(path), DBConnector()
	{
	}
	string IntMapper::Get(int key)
	{
		string value;
		this->GetValue(std::to_string(key), &value);
		return value;
	}
	void IntMapper::Put(int key, string value)
	{
		this->PutValue(Slice(std::to_string(key)), Slice(value));
	}
}