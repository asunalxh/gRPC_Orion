#include "RocksDBConnector.h"
#include <iostream>

using rocksdb::Options;
using rocksdb::ReadOptions;
using rocksdb::WriteOptions;
using std::cout;

namespace RocksDBConnector
{

	BaseConnector::BaseConnector(DB *db)
	{
		this->db = db;
	}
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

	bool BaseConnector::SearchValue(Slice key, string *value)
	{
		rocksdb::Status status = db->Get(ReadOptions(), key, value);
		if (!status.ok())
		{
			//cout << "Get Value Error: " << status.ToString() << '\n';
			return false;
		}
		return true;
	}

	bool BaseConnector::InsertValue(Slice key, Slice value)
	{
		rocksdb::Status status = db->Put(WriteOptions(), key, value);
		if (!status.ok())
		{
			cout << "Put Value Error: " << status.ToString() << '\n';
			false;
		}
		return true;
	}

	bool BaseConnector::DeleteValue(Slice key)
	{
		rocksdb::Status status = db->Delete(WriteOptions(), key);
		if (!status.ok())
		{
			cout << "Delete Value Error: " << status.ToString() << '\n';
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

	bool StringMapper::Get(string key, string &value)
	{
		return this->SearchValue(key, &value);
	}

	bool StringMapper::Put(string key, string value)
	{
		return this->InsertValue(Slice(key), Slice(value));
	}

	bool StringMapper::Delete(string key)
	{
		return this->DeleteValue(key);
	}

	// IntMapper

	IntMapper::IntMapper(string path) : BaseConnector(path), DBConnector()
	{
	}
	bool IntMapper::Get(int key, string &value)
	{
		return this->SearchValue(std::to_string(key), &value);
	}
	bool IntMapper::Put(int key, string value)
	{
		return this->InsertValue(Slice(std::to_string(key)), Slice(value));
	}

	bool IntMapper::Delete(int key)
	{
		return this->DeleteValue(std::to_string(key));
	}

	// IntStorage

	IntStorage::IntStorage(string path) : BaseConnector(path), DBMap()
	{
	}

	bool IntStorage::Get(string key, int &value)
	{
		string str;
		bool flag = this->SearchValue(key, &str);
		if (flag)
		{
			value = std::stoi(str);
			return true;
		}
		return false;
	}
	bool IntStorage::Put(string key, int value)
	{
		return this->InsertValue(key, std::to_string(value));
	}

	bool IntStorage::Delete(string key)
	{
		return this->DeleteValue(key);
	}
}