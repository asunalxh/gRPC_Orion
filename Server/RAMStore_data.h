#pragma once
#include "../common/data_type2.h"
#include <map>
#include <array>
#include "../common/RocksDBConnector.h"

class RAMStore
{
	std::vector<BUCKET> data; // data is a collection of BUCKET
	size_t size;
	size_t emptyNodes; // should we init the 'emptyNodes' as leaf

	DBConnector<int, string> *conn = nullptr;

public:
	RAMStore(DBConnector<int, string> *conn);
	~RAMStore();

	BUCKET Read(size_t pos);
	void Write(size_t pos, BUCKET b);

	// void ReduceEmptyNumbers();
	// size_t GetEmptySize();
};
